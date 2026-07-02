#pragma once

#include <RadianceCascade/RadianceCascadeFeatureProcessorInterface.h>
#include <RadianceCascade/Render/CascadeCommon.h>   // defines MaxCascadeLevels
#include <Atom/RPI.Public/Image/AttachmentImage.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Matrix4x4.h>
#include <AzCore/Name/Name.h>

namespace AZ::RPI { class RenderPipeline; }

namespace RadianceCascade
{
    class CascadeFeatureProcessor final : public RadianceCascadeFeatureProcessorInterface
    {
    public:
        AZ_RTTI(CascadeFeatureProcessor, "{A3F7C29E-9B2D-4A1E-8F5E-6D7C1234ABCD}", RadianceCascadeFeatureProcessorInterface);
        AZ_CLASS_ALLOCATOR(CascadeFeatureProcessor, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);
        CascadeFeatureProcessor() = default;

        void Activate() override;
        void Deactivate() override;
        void Simulate(const FeatureProcessor::SimulatePacket& packet) override;
        void Render(const FeatureProcessor::RenderPacket& packet) override;
        void AddRenderPasses(AZ::RPI::RenderPipeline* renderPipeline) override;

        // Interface getters
        AZ::Data::Instance<AZ::RPI::Image> GetProbeSHBuffer(uint32_t cascadeLevel) const override;
        AZ::Data::Instance<AZ::RPI::Image> GetProbeOctahedralMap() const override;
        const AZStd::array<uint32_t, MaxCascadeLevels>& GetActiveProbeCounts() const override;
        InjectionMode GetInjectionMode() const override;
        void ResetAllProbes() override;
        void SetCameraTransform(const AZ::Transform& worldTransform) override;
        void SetConfiguration(const RadianceCascadeComponentConfig& config) override;

        // FP-owned persistent images
        AZ::Data::Instance<AZ::RPI::AttachmentImage> GetProbeSHAttachment(uint32_t level) const { return m_probeAttachments[level]; }
        AZ::Data::Instance<AZ::RPI::AttachmentImage> GetProbeHistoryAttachment(uint32_t level) const { return m_probeHistoryAttachments[level]; }
        AZ::Data::Instance<AZ::RPI::AttachmentImage> GetDiffuseGIOutput() const { return m_diffuseGIOutput; }
        AZ::Data::Instance<AZ::RPI::AttachmentImage> GetResolvedDepthImage() const { return m_resolvedDepthImage; }

    private:
        void AllocateProbeBuffers();
        void UpdateClipmap();
        void ScheduleProbeUpdates();
        void UpdateViewProjectionMatrix();

        // CVar mirrors
        int32_t m_injectionModeCVar = 0;
        float m_temporalBlendWeight = 0.08f;
        uint32_t m_probesPerFrame = 64;
        bool m_resetRequested = false;

        AZ::Transform m_cameraTransform;
        AZStd::array<AZ::Vector3, MaxCascadeLevels> m_clipmapOrigins;
        AZStd::array<AZ::Vector3, MaxCascadeLevels> m_clipmapCellSizes;

        // Current and history probe attachments
        AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, MaxCascadeLevels> m_probeAttachments;
        AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, MaxCascadeLevels> m_probeHistoryAttachments;

        AZ::Data::Instance<AZ::RPI::AttachmentImage> m_diffuseGIOutput;
        AZ::Data::Instance<AZ::RPI::AttachmentImage> m_resolvedDepthImage;

        AZStd::array<uint32_t, MaxCascadeLevels> m_activeProbes = {};
        bool m_historyValid = false;

        RadianceCascadeComponentConfig m_config;

        AZ::Matrix4x4 m_viewProjMatrix = AZ::Matrix4x4::CreateIdentity();
        AZ::Vector3 m_sunDirection   = AZ::Vector3(0.5f, -1.0f, 0.3f).GetNormalized();
        AZ::Vector3 m_sunColor       = AZ::Vector3(1.0f, 0.95f, 0.8f);
        float m_sunIntensity         = 4.0f;
        float m_ambientIntensity     = 0.1f;

        // Static sizes for probe images (L0: 144x16x16, L1: 72x8x8, L2: 36x4x4)
        static const AZ::RHI::Size ProbeImageSizes[MaxCascadeLevels];
    };
}