
#include "RadianceCascadeModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <RadianceCascade/RadianceCascadeTypeIds.h>

#include <Clients/RadianceCascadeSystemComponent.h>

namespace RadianceCascade
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(RadianceCascadeModuleInterface,
        "RadianceCascadeModuleInterface", RadianceCascadeModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(RadianceCascadeModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(RadianceCascadeModuleInterface, AZ::SystemAllocator);

    RadianceCascadeModuleInterface::RadianceCascadeModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            RadianceCascadeSystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList RadianceCascadeModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<RadianceCascadeSystemComponent>(),
        };
    }
} // namespace RadianceCascade
