#include <AzCore/Module/Module.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <Clients/RadianceCascadeSystemComponent.h>
#include <Tools/RadianceCascadeEditorSystemComponent.h>
#include <Components/RadianceCascadeComponent.h>
#include <Tools/Components/EditorRadianceCascadeComponent.h>

namespace RadianceCascade
{
    class RadianceCascadeEditorModule : public AZ::Module
    {
    public:
        AZ_RTTI(RadianceCascadeEditorModule, "{C1D2E3F4-A5B6-7890-CDEF-012345678901}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RadianceCascadeEditorModule, AZ::SystemAllocator);

        RadianceCascadeEditorModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                RadianceCascadeSystemComponent::CreateDescriptor(),
                RadianceCascadeEditorSystemComponent::CreateDescriptor(),
                RadianceCascadeComponent::CreateDescriptor(),
                EditorRadianceCascadeComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<RadianceCascadeSystemComponent>(),
                azrtti_typeid<RadianceCascadeEditorSystemComponent>()
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(Gem_RadianceCascade_Editor, RadianceCascade::RadianceCascadeEditorModule)