#include <RadianceCascade/Render/CascadeFeatureProcessor.h>

#include <AzCore/Console/Console.h>
#include <AzCore/Math/MathUtils.h>
#include <AzCore/Debug/Trace.h>

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

    // Image sizes for each cascade level (grid size × 9 coefficients)
    // Level 0: 16³ → 144×16×16   Level 1: 8³ → 72×8×8
    // Level 2: 4³ → 36×4×4       Level 3: 2³ → 18×2×2   Level 4: 1³ → 9×1×1
    static const AZ::RHI::Size ProbeImageSizes[MaxCascadeLevels] =
    {
        AZ::RHI::Size(144, 16, 16),
        AZ::RHI::Size( 72,  8,  8),
        AZ::RHI::Size( 36,  4,  4),
        AZ::RHI::Size( 18,  2,  2),
        AZ::RHI::Size(  9,  1,  1)
    };

    void CascadeFeatureProcessor::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            serialize->Class<CascadeFeatureProcessor, AZ::RPI::FeatureProcessor>()->Version(0);
    }

    void CascadeFeatureProcessor::Activate()
    {
        m_historyValid = false;

        // Create one persistent 3D image per cascade level.
        auto* pool = AZ::RPI::ImageSystemInterface::Get()->GetSystemAttachmentPool().get();
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
            AZ_Error("RadianceCascade", m_probeAttachments[level],
                "Failed to create CascadeProbeSH_L%u", level);
        }
    }

    void CascadeFeatureProcessor::Deactivate()
    {
        m_probeSH.fill({});
        m_probeOctahedral = {};
        m_probeHistorySH.fill({});
        m_probeOctahedralHistory = {};
        m_probeAttachments.fill({});
    }

    void CascadeFeatureProcessor::Simulate(const FeatureProcessor::SimulatePacket& packet)
    {
        AZ_UNUSED(packet);
        m_injectionModeCVar = static_cast<int32_t>(r_radianceCascade_mode);
        m_temporalBlendWeight = r_radianceCascade_temporalWeight;

        if (m_resetRequested)
        {
            m_historyValid = false;
            m_resetRequested = false;
        }

        UpdateClipmap();
        UpdateViewProjectionMatrix();
        ScheduleProbeUpdates();
    }

    void CascadeFeatureProcessor::Render(const FeatureProcessor::RenderPacket& packet)
    {
        AZ_UNUSED(packet);
    }

    void CascadeFeatureProcessor::AddRenderPasses(AZ::RPI::RenderPipeline* renderPipeline)
    {
        auto* passSystem = AZ::RPI::PassSystemInterface::Get();

        AZ::RPI::PassFilter depthFilter =
            AZ::RPI::PassFilter::CreateWithPassName(AZ::Name("DepthPrePass"), renderPipeline);
        if (!passSystem->FindFirstPass(depthFilter))
            return;

        // --------------------------------------------------------------------
        // Inject passes for cascade levels 0, 1, 2.
        // --------------------------------------------------------------------
        struct CascadeInfo
        {
            const char* assetPath;
            const char* templateName;
            const char* passName;
        };

        const CascadeInfo cascades[] =
        {
            { "Passes/CascadeInjectPassTemplate_C0.pass", "CascadeInjectPassTemplate_C0", "CascadeProbeSHPass_C0" },
            { "Passes/CascadeInjectPassTemplate_C1.pass", "CascadeInjectPassTemplate_C1", "CascadeProbeSHPass_C1" },
            { "Passes/CascadeInjectPassTemplate_C2.pass", "CascadeInjectPassTemplate_C2", "CascadeProbeSHPass_C2" }
        };

        for (const auto& info : cascades)
        {
            AZ::RPI::PassFilter alreadyThere =
                AZ::RPI::PassFilter::CreateWithPassName(AZ::Name(info.passName), renderPipeline);
            if (passSystem->FindFirstPass(alreadyThere))
                continue;

            AZ::Data::Asset<AZ::RPI::PassAsset> asset =
                AZ::RPI::AssetUtils::LoadAssetByProductPath<AZ::RPI::PassAsset>(
                    info.assetPath, AZ::RPI::AssetUtils::TraceLevel::Warning);
            if (asset.IsReady())
            {
                const auto& tmpl = asset->GetPassTemplate();
                if (tmpl && !passSystem->GetPassTemplate(AZ::Name(tmpl->m_name)))
                {
                    passSystem->AddPassTemplate(AZ::Name(tmpl->m_name),
                        AZStd::make_shared<AZ::RPI::PassTemplate>(*tmpl));
                }
            }

            AZ::RPI::PassRequest req;
            req.m_passName     = info.passName;
            req.m_templateName = info.templateName;

            AZ::RPI::Ptr<AZ::RPI::Pass> pass = passSystem->CreatePassFromRequest(&req);
            if (pass && renderPipeline->AddPassAfter(pass, AZ::Name("ForwardSubsurface")))
            {
                AZ_Printf("RadianceCascade", "Inject pass %s inserted.\n", info.passName);
            }
            else
            {
                AZ_Error("RadianceCascade", false, "Failed to insert %s", info.passName);
            }
        }

        // --------------------------------------------------------------------
        // Merge passes: 16->8 and 8->4.
        // --------------------------------------------------------------------
        struct MergeInfo
        {
            const char* assetPath;
            const char* templateName;
            const char* passName;
        };

        const MergeInfo merges[] =
        {
            { "Passes/CascadeMergePassTemplate_16_8.pass", "CascadeMergePassTemplate_16_8", "CascadeMergePass_16_8" },
            { "Passes/CascadeMergePassTemplate_8_4.pass",   "CascadeMergePassTemplate_8_4",   "CascadeMergePass_8_4"   }
        };

        for (const auto& info : merges)
        {
            AZ::RPI::PassFilter alreadyThere =
                AZ::RPI::PassFilter::CreateWithPassName(AZ::Name(info.passName), renderPipeline);
            if (passSystem->FindFirstPass(alreadyThere))
                continue;

            AZ::Data::Asset<AZ::RPI::PassAsset> asset =
                AZ::RPI::AssetUtils::LoadAssetByProductPath<AZ::RPI::PassAsset>(
                    info.assetPath, AZ::RPI::AssetUtils::TraceLevel::Warning);
            if (asset.IsReady())
            {
                const auto& tmpl = asset->GetPassTemplate();
                if (tmpl && !passSystem->GetPassTemplate(AZ::Name(tmpl->m_name)))
                {
                    passSystem->AddPassTemplate(AZ::Name(tmpl->m_name),
                        AZStd::make_shared<AZ::RPI::PassTemplate>(*tmpl));
                }
            }

            AZ::RPI::PassRequest req;
            req.m_passName     = info.passName;
            req.m_templateName = info.templateName;

            AZ::RPI::Ptr<AZ::RPI::Pass> pass = passSystem->CreatePassFromRequest(&req);
            if (pass && renderPipeline->AddPassAfter(pass, AZ::Name("CascadeProbeSHPass_C2")))  // after last inject
            {
                AZ_Printf("RadianceCascade", "Merge pass %s inserted.\n", info.passName);
            }
            else
            {
                AZ_Error("RadianceCascade", false, "Failed to insert merge %s", info.passName);
            }
        }
    }

    void CascadeFeatureProcessor::AllocateProbeBuffers() {}

    void CascadeFeatureProcessor::UpdateClipmap()
    {
        float spacing = 1.0f;
        m_clipmapOrigins[0] = AZ::Vector3(0.0f);
        m_clipmapCellSizes[0] = AZ::Vector3(spacing);
        for (uint32_t i = 1; i < MaxCascadeLevels; ++i)
        {
            m_clipmapCellSizes[i] = m_clipmapCellSizes[0] * static_cast<float>(1 << i);
            m_clipmapOrigins[i] = m_clipmapOrigins[0];
        }
    }

    void CascadeFeatureProcessor::ScheduleProbeUpdates()
    {
        for (uint32_t i = 0; i < MaxCascadeLevels; ++i)
            m_activeProbes[i] = MaxProbesPerLevel[i];
    }

    void CascadeFeatureProcessor::UpdateViewProjectionMatrix()
    {
        const float fov = AZ::DegToRad(70.0f);
        const float aspect = 16.0f / 9.0f;
        const float nearDist = 0.1f;
        const float farDist = 1000.0f;

        float yScale = 1.0f / tanf(fov * 0.5f);
        float xScale = yScale / aspect;
        float fRange = farDist / (farDist - nearDist);

        AZ::Matrix4x4 proj = AZ::Matrix4x4::CreateIdentity();
        proj.SetRow(0, AZ::Vector4(xScale, 0.0f, 0.0f, 0.0f));
        proj.SetRow(1, AZ::Vector4(0.0f, yScale, 0.0f, 0.0f));
        proj.SetRow(2, AZ::Vector4(0.0f, 0.0f, fRange, 1.0f));
        proj.SetRow(3, AZ::Vector4(0.0f, 0.0f, -nearDist * fRange, 0.0f));

        AZ::Matrix4x4 view = AZ::Matrix4x4::CreateFromTransform(m_cameraTransform).GetInverseFast();
        m_viewProjMatrix = view * proj;
    }

    AZ::Data::Instance<AZ::RPI::Image> CascadeFeatureProcessor::GetProbeSHBuffer(uint32_t cascadeLevel) const
    {
        return (cascadeLevel < MaxCascadeLevels) ? m_probeSH[cascadeLevel]
                                                 : AZ::Data::Instance<AZ::RPI::Image>();
    }

    AZ::Data::Instance<AZ::RPI::Image> CascadeFeatureProcessor::GetProbeOctahedralMap() const
    {
        return m_probeOctahedral;
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
        AZ_Printf("RadianceCascade",
            "Configuration updated (spacing=%.2f, volume=(%.1f,%.1f,%.1f), mode=%s)",
            config.m_probeSpacing,
            static_cast<float>(config.m_volumeSize.GetX()),
            static_cast<float>(config.m_volumeSize.GetY()),
            static_cast<float>(config.m_volumeSize.GetZ()),
            (config.m_injectionMode == InjectionMode::Software ? "Software" : "HWRT"));
    }

    AZ::Data::Instance<AZ::RPI::AttachmentImage> CascadeFeatureProcessor::GetProbeSHAttachment(uint32_t level) const
    {
        if (level < MaxCascadeLevels)
            return m_probeAttachments[level];
        return nullptr;
    }
}