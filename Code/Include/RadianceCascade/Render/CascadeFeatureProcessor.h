#pragma once
#include <RadianceCascade/RadianceCascadeFeatureProcessorInterface.h>
#include <RadianceCascade/Render/ProbeGrid.h>
#include <Atom/RPI.Public/Image/AttachmentImage.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

namespace RadianceCascade
{
    class CascadeFeatureProcessor final 
        : public RadianceCascadeFeatureProcessorInterface
    {
    public:
        AZ_RTTI(CascadeFeatureProcessor, "{A3F7C29E-9B2D-4A1E-8F5E-6D7C1234ABCD}", RadianceCascadeFeatureProcessorInterface);
        AZ_CLASS_ALLOCATOR(CascadeFeatureProcessor, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);

        void Activate() override;
        void Deactivate() override;
        void Simulate(const FeatureProcessor::SimulatePacket& packet) override;
        void Render(const FeatureProcessor::RenderPacket& packet) override;
        void AddRenderPasses(AZ::RPI::RenderPipeline* renderPipeline) override;

        void Enable() override;
        void Disable() override;

        const CascadeLevel& GetLevel(uint32_t i) const override;
        AZ::Data::Instance<AZ::RPI::AttachmentImage> GetRadianceImage(uint32_t i) const override;

    private:
        void AllocateResources();
        void ReleaseResources();

        bool m_isEnabled = false;
        AZStd::unique_ptr<ProbeGrid> m_probeGrid;
        AZStd::array<CascadeLevel, MaxCascadeLevels> m_levels;
        AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, MaxCascadeLevels> m_radianceImages;
    };
}