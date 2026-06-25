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

namespace RadianceCascade
{
    AZ_CVAR(int32_t, r_radianceCascade_mode, 0, nullptr, AZ::ConsoleFunctorFlags::Null,
        "0 = Software injection, 1 = Hardware Ray Tracing");
    AZ_CVAR(float, r_radianceCascade_temporalWeight, 0.08f, nullptr, AZ::ConsoleFunctorFlags::Null,
        "Base temporal blend weight for probe accumulation");

    void CascadeFeatureProcessor::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            serialize->Class<CascadeFeatureProcessor, AZ::RPI::FeatureProcessor>()->Version(0);
    }

    void CascadeFeatureProcessor::Activate()
    {
        m_historyValid = false;
    }

    void CascadeFeatureProcessor::Deactivate()
    {
        m_probeSH.fill({});
        m_probeOctahedral = {};
        m_probeHistorySH.fill({});
        m_probeOctahedralHistory = {};
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

        // ---------- Probe SH injection pass ----------
        AZ::RPI::PassFilter probeAlreadyInjected =
            AZ::RPI::PassFilter::CreateWithPassName(AZ::Name("CascadeProbeSHPass"), renderPipeline);
        if (!passSystem->FindFirstPass(probeAlreadyInjected))
        {
            AZ::Data::Asset<AZ::RPI::PassAsset> probeAsset =
                AZ::RPI::AssetUtils::LoadAssetByProductPath<AZ::RPI::PassAsset>(
                    "Passes/CascadeInjectPassTemplate.pass",
                    AZ::RPI::AssetUtils::TraceLevel::Warning);
            if (probeAsset.IsReady())
            {
                const auto& templateOwner = probeAsset->GetPassTemplate();
                if (templateOwner)
                {
                    passSystem->AddPassTemplate(
                        AZ::Name(templateOwner->m_name),
                        AZStd::make_shared<AZ::RPI::PassTemplate>(*templateOwner));
                }
            }

            AZ::RPI::PassFilter forwardFilter =
                AZ::RPI::PassFilter::CreateWithPassName(AZ::Name("Forward"), renderPipeline);
            if (passSystem->FindFirstPass(forwardFilter))
            {
                AZ::RPI::PassRequest probeRequest;
                probeRequest.m_passName     = "CascadeProbeSHPass";
                probeRequest.m_templateName = "CascadeInjectPassTemplate";

                AZ::RPI::Ptr<AZ::RPI::Pass> probePass = passSystem->CreatePassFromRequest(&probeRequest);
                if (probePass)
                {
                    if (renderPipeline->AddPassAfter(probePass, AZ::Name("ForwardSubsurface")))
                    {
                        AZ_Printf("RadianceCascade", "Probe SH injection pass inserted.\n");
                    }
                    else
                    {
                        AZ_Error("RadianceCascade", false, "AddPassAfter(ForwardSubsurface) failed.");
                    }
                }
                else
                {
                    AZ_Error("RadianceCascade", false, "Failed to create CascadeProbeSHPass from request.");
                }
            }
        }

        // ---------- Cascade merge pass ----------
        AZ::RPI::PassFilter mergeAlreadyInjected =
            AZ::RPI::PassFilter::CreateWithPassName(AZ::Name("CascadeMergePass"), renderPipeline);
        if (!passSystem->FindFirstPass(mergeAlreadyInjected))
        {
            AZ::Data::Asset<AZ::RPI::PassAsset> mergeAsset =
                AZ::RPI::AssetUtils::LoadAssetByProductPath<AZ::RPI::PassAsset>(
                    "Passes/CascadeMergePassTemplate.pass",
                    AZ::RPI::AssetUtils::TraceLevel::Warning);
            if (mergeAsset.IsReady())
            {
                const auto& templateOwner = mergeAsset->GetPassTemplate();
                if (templateOwner)
                {
                    passSystem->AddPassTemplate(
                        AZ::Name(templateOwner->m_name),
                        AZStd::make_shared<AZ::RPI::PassTemplate>(*templateOwner));
                }
            }

            AZ::RPI::PassRequest mergeRequest;
            mergeRequest.m_passName     = "CascadeMergePass";
            mergeRequest.m_templateName = "CascadeMergePassTemplate";

            AZ::RPI::Ptr<AZ::RPI::Pass> mergePass = passSystem->CreatePassFromRequest(&mergeRequest);
            if (mergePass)
            {
                if (renderPipeline->AddPassAfter(mergePass, AZ::Name("CascadeProbeSHPass")))
                {
                    // Connect the fine input to the probe output of the inject pass
                    mergePass->ChangeConnection(AZ::Name("FineProbeInput"),
                        AZ::Name("CascadeProbeSHPass"), AZ::Name("ProbeSHOutput"), renderPipeline);
                    // The coarse input currently binds to its own transient image (which starts black).
                    // Later, we'll connect it to a coarser cascade's inject pass output.
                    AZ_Printf("RadianceCascade", "Cascade merge pass inserted after probe injection.\n");
                }
                else
                {
                    AZ_Error("RadianceCascade", false, "AddPassAfter(CascadeProbeSHPass) failed.");
                }
            }
            else
            {
                AZ_Error("RadianceCascade", false, "Failed to create CascadeMergePass from request.");
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
}