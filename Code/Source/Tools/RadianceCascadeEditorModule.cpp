
#include <RadianceCascade/RadianceCascadeTypeIds.h>
#include <RadianceCascadeModuleInterface.h>
#include "RadianceCascadeEditorSystemComponent.h"
#include "Components/EditorRadianceCascadeComponent.h"

namespace RadianceCascade
{
    class RadianceCascadeEditorModule
        : public RadianceCascadeModuleInterface
    {
    public:
        AZ_RTTI(RadianceCascadeEditorModule, RadianceCascadeEditorModuleTypeId, RadianceCascadeModuleInterface);
        AZ_CLASS_ALLOCATOR(RadianceCascadeEditorModule, AZ::SystemAllocator);

        RadianceCascadeEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                RadianceCascadeEditorSystemComponent::CreateDescriptor(),
                RadianceCascadeComponent::CreateDescriptor(),
                EditorRadianceCascadeComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<RadianceCascadeEditorSystemComponent>(),
            };
        }
    };
}// namespace RadianceCascade

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), RadianceCascade::RadianceCascadeEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_RadianceCascade_Editor, RadianceCascade::RadianceCascadeEditorModule)
#endif
