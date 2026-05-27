#pragma once
#include <AzCore/RTTI/RTTI.h>
#include <Components/RadianceCascadeComponentConfig.h>
#include <AzCore/Math/Vector3.h>

namespace RadianceCascade
{
    class RadianceCascadeComponentController
    {
    public:
        AZ_RTTI(RadianceCascadeComponentController, "{A1B2C3D4-1234-5678-9ABC-DEF012345678}");
        AZ_CLASS_ALLOCATOR(RadianceCascadeComponentController, AZ::SystemAllocator);

        RadianceCascadeComponentController() = default;
        explicit RadianceCascadeComponentController(const RadianceCascadeComponentConfig& config);
        virtual ~RadianceCascadeComponentController() = default;

        static void Reflect(AZ::ReflectContext* context);

        RadianceCascadeComponentConfig GetConfiguration() const;
        void SetConfiguration(const RadianceCascadeComponentConfig& config);

        void Activate() {}
        void Deactivate() {}

        // Editor-visible members (mirrored to config)
        InjectionMode m_injectionMode = InjectionMode::Software;
        float m_probeSpacing = 1.0f;
        AZ::Vector3 m_volumeSize = AZ::Vector3(20.0f);
        float m_temporalWeight = 0.08f;

    private:
        RadianceCascadeComponentConfig m_configuration;
    };
}