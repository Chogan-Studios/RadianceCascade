#pragma once
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/RTTI/ReflectContext.h>
#include <Components/RadianceCascadeComponentConfig.h>

namespace RadianceCascade
{
    class RadianceCascadeComponentController
    {
    public:
        AZ_RTTI(RadianceCascadeComponentController, "{A1B2C3D4-1234-5678-9ABC-DEF012345678}");
        AZ_CLASS_ALLOCATOR(RadianceCascadeComponentController, AZ::SystemAllocator);

        RadianceCascadeComponentController() = default;
        explicit RadianceCascadeComponentController(const RadianceCascadeComponentConfig&) {}

        // Now a declaration, implemented in the .cpp file
        static void Reflect(AZ::ReflectContext* context);

        RadianceCascadeComponentConfig GetConfiguration() const { return RadianceCascadeComponentConfig(); }
        void SetConfiguration(const RadianceCascadeComponentConfig&) {}

        void Activate() {}
        void Deactivate() {}
    };
}