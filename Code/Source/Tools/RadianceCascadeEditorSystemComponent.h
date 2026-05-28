#pragma once
#include <AzCore/Component/Component.h>

namespace RadianceCascade
{
    class RadianceCascadeEditorSystemComponent : public AZ::Component
    {
    public:
        AZ_COMPONENT(RadianceCascadeEditorSystemComponent, "{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}");

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        void Activate() override;
        void Deactivate() override;
    };
}