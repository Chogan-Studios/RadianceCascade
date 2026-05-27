#pragma once
#include <AzCore/RTTI/RTTI.h>
#include <Components/RadianceCascadeComponentConfig.h>

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

        const RadianceCascadeComponentConfig& GetConfiguration() const { return m_configuration; }
        void SetConfiguration(const RadianceCascadeComponentConfig& config) { m_configuration = config; }

        void Activate() {}
        void Deactivate() {}

    private:
        RadianceCascadeComponentConfig m_configuration;
    };
}