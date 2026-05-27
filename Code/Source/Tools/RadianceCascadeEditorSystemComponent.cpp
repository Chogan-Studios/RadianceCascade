#include "RadianceCascadeEditorSystemComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <Clients/RadianceCascadeSystemComponent.h>

namespace RadianceCascade
{
    void RadianceCascadeEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ_UNUSED(context);
    }

    void RadianceCascadeEditorSystemComponent::Activate()
    {
    }

    void RadianceCascadeEditorSystemComponent::Deactivate()
    {
    }

    void RadianceCascadeEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("RadianceCascadeEditorService"));
    }

    void RadianceCascadeEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("RadianceCascadeEditorService"));
    }

    void RadianceCascadeEditorSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("RadianceCascadeService"));
    }

    void RadianceCascadeEditorSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        AZ_UNUSED(dependent);
    }
}