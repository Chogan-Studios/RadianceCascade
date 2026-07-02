#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <AzCore/Console/Console.h>
#include <AzCore/Math/MathUtils.h>
#include <Atom/RPI.Public/RenderPipeline.h>
#include <Atom/RPI.Public/Pass/PassFilter.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <Atom/RPI.Reflect/Pass/PassRequest.h>
#include <Atom/RPI.Reflect/Pass/PassAsset.h>
#include <Atom/RPI.Reflect/Asset/AssetUtils.h>
#include <Atom/RPI.Public/Image/ImageSystemInterface.h>
#include <Atom/RPI.Public/Image/AttachmentImagePool.h>

namespace RadianceCascade
{
    AZ_CVAR(int32_t, r_radianceCascade_mode, 0, nullptr, AZ::ConsoleFunctorFlags::Null,
        "0 = Software injection, 1 = Hardware Ray Tracing");
    AZ_CVAR(float, r_radianceCascade_temporalWeight, 0.08f, nullptr, AZ::ConsoleFunctorFlags::Null,
        "Base temporal blend weight for probe accumulation");

    const AZ::RHI::Size CascadeFeatureProcessor::ProbeImageSizes[MaxCascadeLevels] =
    {
        AZ::RHI::Size(144, 16, 16),
        AZ::RHI::Size( 72,  8,  8),
        AZ::RHI::Size( 36,  4,  4),
    };

    void CascadeFeatureProcessor::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<CascadeFeatureProcessor, AZ::RPI::FeatureProcessor>()->Version(0);
        }
    }

    void CascadeFeatureProcessor::Activate()
    {
        auto* pool = AZ::RPI::ImageSystemInterface::Get()->GetSystemAttachmentPool().get();
        if (!pool)
        {
            AZ_Error("RadianceCascade", false, "Failed to get attachment pool");
            return;
        }

        // Probe buffers
        for (uint32_t level = 0; level < MaxCascadeLevels; ++level)
        {
            AZ::RHI::ImageDescriptor desc;
            desc.m_bindFlags = AZ::RHI::ImageBindFlags::ShaderReadWrite;
            desc.m_dimension = AZ::RHI::ImageDimension::Image3D;
            desc.m_size      = ProbeImageSizes[level];
            desc.m_format    = AZ::RHI::Format::R32G32B32A32_FLOAT;
            desc.m_arraySize = 1;
            desc.m_mipLevels = 1;

            AZ::RPI::CreateAttachmentImageRequest req;
            req.m_imageDescriptor = desc;
            req.m_imageName       = AZ::Name(AZStd::string::format("CascadeProbeSH_L%u", level));
            req.m_isUniqueName    = true;
            req.m_imagePool       = pool;
            m_probeAttachments[level] = AZ::RPI::AttachmentImage::Create(req);

            AZ::RPI::CreateAttachmentImageRequest histReq;
            histReq.m_imageDescriptor = desc;
            histReq.m_imageName       = AZ::Name(AZStd::string::format("CascadeProbeSHHistory_L%u", level));
            histReq.m_isUniqueName    = true;
            histReq.m_imagePool       = pool;
            m_probeHistoryAttachments[level] = AZ::RPI::AttachmentImage::Create(histReq);
        }

        // Diffuse GI output
        {
            AZ::RHI::ImageDescriptor desc;
            desc.m_bindFlags = AZ::RHI::ImageBindFlags::ShaderReadWrite;
            desc.m_dimension = AZ::RHI::ImageDimension::Image2D;
            desc.m_size      = AZ::RHI::Size(1920, 1080, 1);
            desc.m_format    = AZ::RHI::Format::R16G16B16A16_FLOAT;
            desc.m_arraySize = 1;
            desc.m_mipLevels = 1;

            AZ::RPI::CreateAttachmentImageRequest req;
            req.m_imageDescriptor = desc;
            req.m_imageName       = AZ::Name("CascadeDiffuseGIOutput");
            req.m_isUniqueName    = true;
            req.m_imagePool       = pool;
            m_diffuseGIOutput = AZ::RPI::AttachmentImage::Create(req);
        }

        // Resolved depth is no longer needed, but we keep the FP-owned image for potential future use.
        // We'll allocate it to avoid errors, but it's not used in the pipeline.
        {
            AZ::RHI::ImageDescriptor desc;
            desc.m_bindFlags = AZ::RHI::ImageBindFlags::ShaderReadWrite;
            desc.m_dimension = AZ::RHI::ImageDimension::Image2D;
            desc.m_size      = AZ::RHI::Size(1920, 1080, 1);
            desc.m_format    = AZ::RHI::Format::R32_FLOAT;
            desc.m_arraySize = 1;
            desc.m_mipLevels = 1;

            AZ::RPI::CreateAttachmentImageRequest req;
            req.m_imageDescriptor = desc;
            req.m_imageName       = AZ::Name("CascadeResolvedDepth");
            req.m_isUniqueName    = true;
            req.m_imagePool       = pool;
            m_resolvedDepthImage = AZ::RPI::AttachmentImage::Create(req);
        }

        m_historyValid = false;
    }

    void CascadeFeatureProcessor::Deactivate()
    {
        m_probeAttachments.fill({});
        m_probeHistoryAttachments.fill({});
        m_diffuseGIOutput = nullptr;
        m_resolvedDepthImage = nullptr;
    }

    void CascadeFeatureProcessor::Simulate([[maybe_unused]] const FeatureProcessor::SimulatePacket& packet)
    {
    }

    void CascadeFeatureProcessor::Render([[maybe_unused]] const FeatureProcessor::RenderPacket& packet)
    {
    }

    void CascadeFeatureProcessor::AddRenderPasses(AZ::RPI::RenderPipeline* renderPipeline)
    {
        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        AZ::Name lastPass = AZ::Name("ForwardSubsurface");

        auto createPass = [&](const char* assetPath, const char* templateName, const char* passName) -> AZ::RPI::Ptr<AZ::RPI::Pass>
        {
            AZ::Data::Asset<AZ::RPI::PassAsset> asset = 
                AZ::RPI::AssetUtils::LoadAssetByProductPath<AZ::RPI::PassAsset>(assetPath, AZ::RPI::AssetUtils::TraceLevel::Warning);
            if (!asset.IsReady())
                return nullptr;
            const auto& owner = asset->GetPassTemplate();
            if (owner && !passSystem->GetPassTemplate(AZ::Name(owner->m_name)))
                passSystem->AddPassTemplate(AZ::Name(owner->m_name), AZStd::make_shared<AZ::RPI::PassTemplate>(*owner));
            AZ::RPI::PassRequest req;
            req.m_passName = passName;
            req.m_templateName = templateName;
            return passSystem->CreatePassFromRequest(&req);
        };

        // Injection passes
        struct CascadeInfo { const char* assetPath; const char* templateName; const char* passName; };
        const CascadeInfo cascades[] = {
            { "Passes/CascadeInjectPassTemplate_C0.pass", "CascadeInjectPassTemplate_C0", "CascadeProbeSHPass_C0" },
            { "Passes/CascadeInjectPassTemplate_C1.pass", "CascadeInjectPassTemplate_C1", "CascadeProbeSHPass_C1" },
            { "Passes/CascadeInjectPassTemplate_C2.pass", "CascadeInjectPassTemplate_C2", "CascadeProbeSHPass_C2" }
        };
        for (const auto& info : cascades)
        {
            AZ::Name passName(info.passName);
            if (!passSystem->FindFirstPass(AZ::RPI::PassFilter::CreateWithPassName(passName, renderPipeline)))
            {
                auto pass = createPass(info.assetPath, info.templateName, info.passName);
                if (pass && renderPipeline->AddPassAfter(pass, lastPass))
                    AZ_Printf("RadianceCascade", "Inject pass %s inserted.", info.passName);
            }
            lastPass = passName;
        }

        // Merge passes
        struct MergeInfo { const char* assetPath; const char* templateName; const char* passName; };
        const MergeInfo merges[] = {
            { "Passes/CascadeMergePassTemplate_16_8.pass", "CascadeMergePassTemplate_16_8", "CascadeMerge_16to8" },
            { "Passes/CascadeMergePassTemplate_8_4.pass",   "CascadeMergePassTemplate_8_4",   "CascadeMerge_8to4"   }
        };
        for (const auto& info : merges)
        {
            AZ::Name passName(info.passName);
            if (!passSystem->FindFirstPass(AZ::RPI::PassFilter::CreateWithPassName(passName, renderPipeline)))
            {
                auto pass = createPass(info.assetPath, info.templateName, info.passName);
                if (pass && renderPipeline->AddPassAfter(pass, lastPass))
                    AZ_Printf("RadianceCascade", "Merge pass %s inserted.", info.passName);
            }
            lastPass = passName;
        }

        // Diffuse GI
        AZ::Name diffuseGIPassName("CascadeDiffuseGIPass");
        if (!passSystem->FindFirstPass(AZ::RPI::PassFilter::CreateWithPassName(diffuseGIPassName, renderPipeline)))
        {
            auto giPass = createPass("Passes/CascadeDiffuseGIPassTemplate.pass", "CascadeDiffuseGIPassTemplate", diffuseGIPassName.GetCStr());
            if (giPass && renderPipeline->AddPassAfter(giPass, lastPass))
                AZ_Printf("RadianceCascade", "Diffuse GI pass inserted.");
        }
    }

    // Getters
    AZ::Data::Instance<AZ::RPI::Image> CascadeFeatureProcessor::GetProbeSHBuffer(uint32_t cascadeLevel) const
    {
        return m_probeAttachments[cascadeLevel];
    }

    AZ::Data::Instance<AZ::RPI::Image> CascadeFeatureProcessor::GetProbeOctahedralMap() const
    {
        return {};
    }

    const AZStd::array<uint32_t, MaxCascadeLevels>& CascadeFeatureProcessor::GetActiveProbeCounts() const
    {
        return m_activeProbes;
    }

    InjectionMode CascadeFeatureProcessor::GetInjectionMode() const
    {
        return static_cast<InjectionMode>(m_injectionModeCVar);
    }

    void CascadeFeatureProcessor::ResetAllProbes()
    {
        m_resetRequested = true;
    }

    void CascadeFeatureProcessor::SetCameraTransform(const AZ::Transform& worldTransform)
    {
        m_cameraTransform = worldTransform;
    }

    void CascadeFeatureProcessor::SetConfiguration(const RadianceCascadeComponentConfig& config)
    {
        m_config = config;
    }

    void CascadeFeatureProcessor::AllocateProbeBuffers() {}
    void CascadeFeatureProcessor::UpdateClipmap() {}
    void CascadeFeatureProcessor::ScheduleProbeUpdates() {}
    void CascadeFeatureProcessor::UpdateViewProjectionMatrix() {}
}