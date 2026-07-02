#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/ComponentBus.h>

namespace RadianceCascade
{
    class RadianceCascadeSystemComponent
        : public AZ::Component
    {
    public:
        AZ_COMPONENT(RadianceCascadeSystemComponent, "{B4C7B3E4-DE5F-4A9D-9F7E-8C3B1A2D4E5F}");

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        void Init() override;
        void Activate() override;
        void Deactivate() override;
    };
}