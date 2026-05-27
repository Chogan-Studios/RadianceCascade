#pragma once
#include <RadianceCascade/RadianceCascadeFeatureProcessorInterface.h>
#include <Atom/RPI.Public/Image/StreamingImage.h>
#include <AzCore/Math/Transform.h>

namespace RadianceCascade
{
    class CascadeFeatureProcessor final : public RadianceCascadeFeatureProcessorInterface
    {
    public:
        AZ_RTTI(CascadeFeatureProcessor, "{A3F7C29E-9B2D-4A1E-8F5E-6D7C1234ABCD}", RadianceCascadeFeatureProcessorInterface);
        AZ_CLASS_ALLOCATOR(CascadeFeatureProcessor, AZ::SystemAllocator);

        CascadeFeatureProcessor() = default;

        void Activate() override;
        void Deactivate() override;
        void Simulate(const FeatureProcessor::SimulatePacket& packet) override;
        void Render(const FeatureProcessor::RenderPacket& packet) override;

        AZ::Data::Instance<AZ::RPI::Image> GetProbeSHBuffer(uint32_t cascadeLevel) const override;
        AZ::Data::Instance<AZ::RPI::Image> GetProbeOctahedralMap() const override;
        const AZStd::array<uint32_t, MaxCascadeLevels>& GetActiveProbeCounts() const override;
        InjectionMode GetInjectionMode() const override;
        void ResetAllProbes() override;
        void SetCameraTransform(const AZ::Transform& worldTransform) override;

    private:
        void AllocateProbeBuffers();
        void UpdateClipmap();
        void ScheduleProbeUpdates();

        int32_t m_injectionModeCVar = 0;
        float m_temporalBlendWeight = 0.08f;
        uint32_t m_probesPerFrame = 64;
        bool m_resetRequested = false;

        AZ::Transform m_cameraTransform;
        AZStd::array<AZ::Vector3, MaxCascadeLevels> m_clipmapOrigins;
        AZStd::array<AZ::Vector3, MaxCascadeLevels> m_clipmapCellSizes;

        AZStd::array<AZ::Data::Instance<AZ::RPI::Image>, MaxCascadeLevels> m_probeSH;
        AZStd::array<AZ::Data::Instance<AZ::RPI::Image>, MaxCascadeLevels> m_probeHistorySH;
        AZ::Data::Instance<AZ::RPI::Image> m_probeOctahedral;
        AZ::Data::Instance<AZ::RPI::Image> m_probeOctahedralHistory;

        AZStd::array<uint32_t, MaxCascadeLevels> m_activeProbes = {};
        bool m_historyValid = false;
    };
}