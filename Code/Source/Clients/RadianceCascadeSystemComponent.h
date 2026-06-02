#pragma once
#include <AzCore/Component/Component.h>

namespace RadianceCascade
{
    class RadianceCascadeSystemComponent : public AZ::Component
    {
    public:
        AZ_COMPONENT(RadianceCascadeSystemComponent, "{3F7D5B9C-2A4E-4D1F-8B6A-9C0E1F2A3B4D}");

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        void Activate() override;
        void Deactivate() override;
    };
}