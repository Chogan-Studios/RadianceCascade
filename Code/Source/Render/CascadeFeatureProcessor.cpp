#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <Atom/RPI.Public/RenderPipeline.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/Image/ImageSystemInterface.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <Atom/RPI.Public/Pass/PassFilter.h>
#include <Atom/RPI.Reflect/Pass/PassRequest.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace RadianceCascade
{
    void ScreenSpaceProbeGrid::Update(const AZ::Matrix4x4& viewProj) { m_viewProj = viewProj; }
    
    AZ::Vector3 ScreenSpaceProbeGrid::WorldToGrid([[maybe_unused]] const AZ::Vector3& worldPos, [[maybe_unused]] uint32_t cascade) const { return worldPos; }
    uint32_t ScreenSpaceProbeGrid::GetGridSize([[maybe_unused]] uint32_t cascade) const { return 64; }
    float ScreenSpaceProbeGrid::GetSpacing([[maybe_unused]] uint32_t cascade) const { return r_rc_probeSpacing * (1 << cascade); }

    void CascadeFeatureProcessor::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<CascadeFeatureProcessor, RadianceCascadeFeatureProcessorInterface>()->Version(1);
        }
    }

    void CascadeFeatureProcessor::Activate()
    {
        float baseSpacing = r_rc_probeSpacing;
        for (uint32_t i = 0; i < MaxCascadeLevels; ++i)
        {
            m_levels[i].m_spacing = baseSpacing * (1 << i);
            m_levels[i].m_intervalStart = m_levels[i].m_spacing;
            m_levels[i].m_intervalEnd = m_levels[i].m_spacing * 2.0f;
            m_levels[i].m_probeGridSize = 32;
        }
    }

    void CascadeFeatureProcessor::Deactivate() 
    { 
        ReleaseResources(); 
    }

    void CascadeFeatureProcessor::Enable()
    {
        if (m_isEnabled) return;
        m_isEnabled = true;
        if (r_rc_mode == 1) m_probeGrid = AZStd::make_unique<ScreenSpaceProbeGrid>();
        AllocateResources();

        AZ_Printf("RadianceCascade", "Feature Processor Enabled. Triggering pipeline rebuild.");

        if (auto* scene = GetParentScene())
        {
            for (auto& pipeline : scene->GetRenderPipelines())
            {
                scene->TryApplyRenderPipelineChanges(pipeline.get());
            }
        }
    }

    void CascadeFeatureProcessor::Disable()
    {
        if (!m_isEnabled) return;
        m_isEnabled = false;
        ReleaseResources();
        m_probeGrid.reset();

        AZ_Printf("RadianceCascade", "Feature Processor Disabled. Triggering pipeline rebuild.");

        if (auto* scene = GetParentScene())
        {
            for (auto& pipeline : scene->GetRenderPipelines())
            {
                scene->TryApplyRenderPipelineChanges(pipeline.get());
            }
        }
    }

    void CascadeFeatureProcessor::AllocateResources()
    {
        auto* pool = AZ::RPI::ImageSystemInterface::Get()->GetSystemAttachmentPool().get();
        if (!pool) return;

        for (uint32_t i = 0; i < MaxCascadeLevels; ++i)
        {
            AZ::RHI::ImageDescriptor desc;
            desc.m_bindFlags = AZ::RHI::ImageBindFlags::ShaderReadWrite;
            desc.m_dimension = AZ::RHI::ImageDimension::Image2D;
            desc.m_size = AZ::RHI::Size(m_levels[i].m_probeGridSize * OctaTexW, m_levels[i].m_probeGridSize * OctaTexH, 1);
            desc.m_format = AZ::RHI::Format::R16G16B16A16_FLOAT;
            desc.m_arraySize = 1;
            desc.m_mipLevels = 1;

            AZ::RPI::CreateAttachmentImageRequest req;
            req.m_imageDescriptor = desc;
            req.m_imageName = AZ::Name(AZStd::string::format("CascadeRadiance_L%u", i));
            req.m_isUniqueName = false;
            req.m_imagePool = pool;
            m_radianceImages[i] = AZ::RPI::AttachmentImage::Create(req);
        }
    }

    void CascadeFeatureProcessor::ReleaseResources() { m_radianceImages.fill({}); }

    void CascadeFeatureProcessor::Simulate([[maybe_unused]] const FeatureProcessor::SimulatePacket& packet)
    {
        if (!m_isEnabled || !m_probeGrid) return;
        m_probeGrid->Update(AZ::Matrix4x4::CreateIdentity());
    }

    void CascadeFeatureProcessor::Render([[maybe_unused]] const FeatureProcessor::RenderPacket& packet) {}

    void CascadeFeatureProcessor::AddRenderPasses(AZ::RPI::RenderPipeline* renderPipeline)
    {
        if (!m_isEnabled) return;
        
        AZ_Printf("RadianceCascade", "AddRenderPasses called for pipeline: %s", renderPipeline->GetId().GetCStr());

        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        if (!passSystem) return;

        AZ::Name lastPass = AZ::Name("ForwardSubsurface");

        auto insertPass = [&](const char* templateName, const char* passName) -> bool
        {
            AZ::Name name(passName);
            if (passSystem->FindFirstPass(AZ::RPI::PassFilter::CreateWithPassName(name, renderPipeline)))
                return true; // Already inserted

            AZ::RPI::PassRequest req;
            req.m_passName = name;
            req.m_templateName = templateName;
            auto pass = passSystem->CreatePassFromRequest(&req);
            if (!pass) 
            {
                AZ_Error("RadianceCascade", false, "Failed to create pass: %s (Template: %s)", passName, templateName);
                return false;
            }

            if (renderPipeline->AddPassAfter(pass, lastPass))
            {
                AZ_Printf("RadianceCascade", "Successfully inserted pass: %s after %s", passName, lastPass.GetCStr());
                lastPass = name;
                return true;
            }
            
            AZ_Error("RadianceCascade", false, "Failed to insert pass %s after %s", passName, lastPass.GetCStr());
            return false;
        };

        insertPass("ClearProbesPassTemplate_C0", "CascadeClear_C0");
        insertPass("ClearProbesPassTemplate_C1", "CascadeClear_C1");
        insertPass("ClearProbesPassTemplate_C2", "CascadeClear_C2");
        
        insertPass("CascadeRayMarchPassTemplate_C0", "CascadeRayMarch_C0");
        insertPass("CascadeRayMarchPassTemplate_C1", "CascadeRayMarch_C1");
        insertPass("CascadeRayMarchPassTemplate_C2", "CascadeRayMarch_C2");

        insertPass("CascadeMergePassTemplate_8_4", "CascadeMerge_8to4");
        insertPass("CascadeMergePassTemplate_16_8", "CascadeMerge_16to8");
        
        insertPass("CascadeResolvePassTemplate", "CascadeResolve");
    }

    const CascadeLevel& CascadeFeatureProcessor::GetLevel(uint32_t i) const { return m_levels[i]; }
    AZ::Data::Instance<AZ::RPI::AttachmentImage> CascadeFeatureProcessor::GetRadianceImage(uint32_t i) const { return m_radianceImages[i]; }
}