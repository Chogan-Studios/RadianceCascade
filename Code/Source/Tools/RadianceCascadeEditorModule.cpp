#include <AzCore/Module/Module.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <Tools/RadianceCascadeEditorSystemComponent.h>
#include <Clients/RadianceCascadeSystemComponent.h>
#include <Components/RadianceCascadeComponent.h>
#include <Tools/Components/EditorRadianceCascadeComponent.h>

namespace RadianceCascade
{
    class RadianceCascadeEditorModule : public AZ::Module
    {
    public:
        AZ_RTTI(RadianceCascadeEditorModule, "{3F7D5B9C-2A4E-4D1F-8B6A-9C0E1F2A3B4D}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RadianceCascadeEditorModule, AZ::SystemAllocator);

        RadianceCascadeEditorModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                RadianceCascadeEditorSystemComponent::CreateDescriptor(),
                RadianceCascadeComponent::CreateDescriptor(),
                EditorRadianceCascadeComponent::CreateDescriptor(),
            });
        }
    };
}

AZ_DECLARE_MODULE_CLASS(Gem_RadianceCascade_Editor, RadianceCascade::RadianceCascadeEditorModule)