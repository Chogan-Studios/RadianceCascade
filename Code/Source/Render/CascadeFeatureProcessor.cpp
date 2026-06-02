#include <RadianceCascade/Render/CascadeFeatureProcessor.h>

#include <AzCore/Console/Console.h>
#include <AzCore/Math/MathUtils.h>
#include <AzCore/Debug/Trace.h>

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
        m_frameCount = 0;
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

        // Print every 60 frames to prove the FP is alive
        if (m_frameCount % 60 == 0)
            AZ_Printf("RadianceCascade", "Frame %llu\n", m_frameCount);
        ++m_frameCount;
    }

    void CascadeFeatureProcessor::Render(const FeatureProcessor::RenderPacket& packet)
    {
        AZ_UNUSED(packet);
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