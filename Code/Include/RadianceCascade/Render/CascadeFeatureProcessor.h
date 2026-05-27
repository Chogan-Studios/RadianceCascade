#pragma once

#include <RadianceCascade/RadianceCascadeFeatureProcessorInterface.h>
#include <Atom/RPI.Public/Image/ImageSystemInterface.h>
#include <Atom/RPI.Public/Image/StreamingImage.h>
#include <AzCore/Math/Transform.h>

namespace RadianceCascade
{
    class CascadeFeatureProcessor final : public RadianceCascadeFeatureProcessorInterface
    {
    public:
        AZ_RTTI(CascadeFeatureProcessor, "{A3F7C29E-9B2D-4A1E-8F5E-6D7C1234ABCD}", RadianceCascadeFeatureProcessorInterface);
        AZ_CLASS_ALLOCATOR(CascadeFeatureProcessor, AZ::SystemAllocator);

        static AZStd::unique_ptr<CascadeFeatureProcessor> Create();

        // FeatureProcessor
        void Activate() override;
        void Deactivate() override;
        void Simulate(const FeatureProcessor::SimulatePacket& packet) override;
        void Render(const FeatureProcessor::RenderPacket& packet) override;

        // Interface
        AZ::Data::Instance<AZ::RPI::Image> GetProbeSHBuffer(uint32_t cascadeLevel) const override;
        AZ::Data::Instance<AZ::RPI::Image> GetProbeOctahedralMap() const override;
        const AZStd::array<uint32_t, MaxCascadeLevels>& GetActiveProbeCounts() const override;
        InjectionMode GetInjectionMode() const override;
        void ResetAllProbes() override;
        void SetCameraTransform(const AZ::Transform& worldTransform) override;

    private:
        CascadeFeatureProcessor() = default;

        void AllocateProbeBuffers();
        void UpdateClipmap();
        void ScheduleProbeUpdates();

        // CVars
        int32_t m_injectionModeCVar = 0;
        float m_temporalBlendWeight = 0.08f;
        uint32_t m_probesPerFrame = 64;
        bool m_resetRequested = false;

        // Camera / clipmap
        AZ::Transform m_cameraTransform;
        AZStd::array<AZ::Vector3, MaxCascadeLevels> m_clipmapOrigins;
        AZStd::array<AZ::Vector3, MaxCascadeLevels> m_clipmapCellSizes;

        // Probe radiance buffers (3D textures, or RWStructuredBuffer alternative)
        AZStd::array<AZ::Data::Instance<AZ::RPI::Image>, MaxCascadeLevels> m_probeSH;
        AZStd::array<AZ::Data::Instance<AZ::RPI::Image>, MaxCascadeLevels> m_probeHistorySH;
        AZ::Data::Instance<AZ::RPI::Image> m_probeOctahedral;
        AZ::Data::Instance<AZ::RPI::Image> m_probeOctahedralHistory;

        AZStd::array<uint32_t, MaxCascadeLevels> m_activeProbes = {};
        bool m_historyValid = false;
    };
}