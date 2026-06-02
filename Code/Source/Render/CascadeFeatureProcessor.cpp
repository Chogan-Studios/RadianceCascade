#include <RadianceCascade/Render/CascadeFeatureProcessor.h>

#include <AzCore/Console/Console.h>
#include <AzCore/Math/MathUtils.h>
#include <AzCore/Debug/Trace.h>

#include <Atom/RPI.Public/RenderPipeline.h>
#include <Atom/RPI.Public/Pass/PassFilter.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <Atom/RPI.Reflect/Pass/PassRequest.h>

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
        if (m_resetRequested) { m_historyValid = false; m_resetRequested = false; }
        UpdateClipmap();
        ScheduleProbeUpdates();
    }

    void CascadeFeatureProcessor::Render(const FeatureProcessor::RenderPacket& packet)
    {
        AZ_UNUSED(packet);
    }

    // -------------------------------------------------------------------------
    // THE MISSING HOOK – called by the scene whenever a pipeline is built
    // -------------------------------------------------------------------------
    void CascadeFeatureProcessor::AddRenderPasses(AZ::RPI::RenderPipeline* renderPipeline)
    {
        AZ_Printf("RadianceCascade", "AddRenderPasses called!\n");

        // Only inject into pipelines that have a DepthPrePass
        AZ::RPI::PassFilter depthFilter =
            AZ::RPI::PassFilter::CreateWithPassName(AZ::Name("DepthPrePass"), renderPipeline);
        if (!AZ::RPI::PassSystemInterface::Get()->FindFirstPass(depthFilter))
            return;

        // Avoid double‑injection
        AZ::RPI::PassFilter alreadyInjected =
            AZ::RPI::PassFilter::CreateWithPassName(AZ::Name("CascadeInjectPass"), renderPipeline);
        if (AZ::RPI::PassSystemInterface::Get()->FindFirstPass(alreadyInjected))
            return;

        // Build the inject pass request
        AZ::RPI::PassRequest injectRequest;
        injectRequest.m_passName     = "CascadeInjectPass";
        injectRequest.m_templateName = "CascadeInjectPassTemplate";

        // Create the pass from the request
        AZ::RPI::Ptr<AZ::RPI::Pass> newPass =
            AZ::RPI::PassSystemInterface::Get()->CreatePassFromRequest(&injectRequest);
        if (!newPass)
        {
            AZ_Error("CascadeFeatureProcessor", false, "Failed to create CascadeInjectPass.");
            return;
        }

        // Insert after DepthPrePass
        renderPipeline->AddPassAfter(newPass, AZ::Name("DepthPrePass"));
        AZ_Printf("RadianceCascade", "CascadeInjectPass inserted into pipeline.\n");
    }

    void CascadeFeatureProcessor::AllocateProbeBuffers() {}
    void CascadeFeatureProcessor::UpdateClipmap()
    {
        m_clipmapOrigins[0] = AZ::Vector3(0.0f);
        m_clipmapCellSizes[0] = AZ::Vector3(1.0f);
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

    AZ::Data::Instance<AZ::RPI::Image> CascadeFeatureProcessor::GetProbeSHBuffer(uint32_t cascadeLevel) const
    { return cascadeLevel < MaxCascadeLevels ? m_probeSH[cascadeLevel] : AZ::Data::Instance<AZ::RPI::Image>(); }
    AZ::Data::Instance<AZ::RPI::Image> CascadeFeatureProcessor::GetProbeOctahedralMap() const
    { return m_probeOctahedral; }
    const AZStd::array<uint32_t, MaxCascadeLevels>& CascadeFeatureProcessor::GetActiveProbeCounts() const
    { return m_activeProbes; }
    InjectionMode CascadeFeatureProcessor::GetInjectionMode() const
    { return static_cast<InjectionMode>(m_injectionModeCVar); }
    void CascadeFeatureProcessor::ResetAllProbes() { m_resetRequested = true; }
    void CascadeFeatureProcessor::SetCameraTransform(const AZ::Transform& worldTransform)
    { m_cameraTransform = worldTransform; }
}