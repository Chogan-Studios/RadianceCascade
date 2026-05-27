#include "CascadeFeatureProcessor.h"
#include <Atom/RPI.Public/RPISystemInterface.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/Image/Image.h>
#include <Atom/RPI.Reflect/Image/ImageDescriptor.h>
#include <AzCore/Console/Console.h>
#include <AzCore/Math/MathUtils.h>

namespace RadianceCascade
{
    AZ_CVAR(int32_t, r_radianceCascade_mode, 0, nullptr, AZ::ConsoleFunctorFlags::Null,
        "0 = Software injection, 1 = Hardware Ray Tracing");
    AZ_CVAR(float, r_radianceCascade_temporalWeight, 0.08f, nullptr, AZ::ConsoleFunctorFlags::Null,
        "Base temporal blend weight for probe accumulation");

    AZStd::unique_ptr<CascadeFeatureProcessor> CascadeFeatureProcessor::Create()
    {
        return AZStd::make_unique<CascadeFeatureProcessor>();
    }

    void CascadeFeatureProcessor::Activate()
    {
        AllocateProbeBuffers();
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
        m_injectionModeCVar = static_cast<int32_t>(r_radianceCascade_mode);
        m_temporalBlendWeight = r_radianceCascade_temporalWeight;

        if (m_resetRequested)
        {
            m_historyValid = false;
            m_resetRequested = false;
        }

        UpdateClipmap();
        ScheduleProbeUpdates();
    }

    void CascadeFeatureProcessor::Render(const FeatureProcessor::RenderPacket& packet)
    {
        // Data made available through pass SRGs, nothing to do here
    }

    void CascadeFeatureProcessor::AllocateProbeBuffers()
    {
        auto* imageSystem = AZ::RPI::ImageSystemInterface::Get();
        for (uint32_t level = 0; level < MaxCascadeLevels; ++level)
        {
            uint32_t dim = static_cast<uint32_t>(ceilf(cbrtf(static_cast<float>(MaxProbesPerLevel[level]))));
            AZ::RPI::ImageDescriptor desc;
            desc.m_size = AZ::RHI::Size(dim, dim, dim);
            desc.m_format = AZ::RHI::Format::R16G16B16A16_FLOAT;
            desc.m_arraySize = 1;
            desc.m_mipLevels = 1;
            desc.m_bindFlags = AZ::RHI::ImageBindFlags::ShaderReadWrite;
            m_probeSH[level] = imageSystem->CreateImage(desc, AZ::Name("ProbeSH_L" + AZStd::to_string(level)));
            m_probeHistorySH[level] = imageSystem->CreateImage(desc, AZ::Name("ProbeHistorySH_L" + AZStd::to_string(level)));
        }

        // Octahedral map for specular cascade
        uint32_t fineProbes = MaxProbesPerLevel[SpecularCascadeIndex];
        uint32_t fineDim = static_cast<uint32_t>(ceilf(cbrtf(static_cast<float>(fineProbes))));
        AZ::RPI::ImageDescriptor octaDesc;
        octaDesc.m_size = AZ::RHI::Size(OctahedralMapSize, OctahedralMapSize, 1);
        octaDesc.m_arraySize = fineDim * fineDim * fineDim;
        octaDesc.m_format = AZ::RHI::Format::R16G16B16A16_FLOAT;
        octaDesc.m_bindFlags = AZ::RHI::ImageBindFlags::ShaderReadWrite;
        m_probeOctahedral = imageSystem->CreateImage(octaDesc, AZ::Name("ProbeOctahedral"));
        m_probeOctahedralHistory = imageSystem->CreateImage(octaDesc, AZ::Name("ProbeOctahedralHistory"));
    }

    void CascadeFeatureProcessor::UpdateClipmap()
    {
        // Placeholder: fixed world origin and 1m spacing
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
        // Will be filled later; for now all probes are considered active and need injection
        for (uint32_t i = 0; i < MaxCascadeLevels; ++i)
            m_activeProbes[i] = MaxProbesPerLevel[i];
    }

    // Accessors
    AZ::Data::Instance<AZ::RPI::Image> CascadeFeatureProcessor::GetProbeSHBuffer(uint32_t cascadeLevel) const
    {
        return cascadeLevel < MaxCascadeLevels ? m_probeSH[cascadeLevel] : AZ::Data::Instance<AZ::RPI::Image>();
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
}