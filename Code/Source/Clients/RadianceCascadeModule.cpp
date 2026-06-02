#include <AzCore/Module/Module.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <Clients/RadianceCascadeSystemComponent.h>
#include <Components/RadianceCascadeComponent.h>

namespace RadianceCascade
{
    class RadianceCascadeModule : public AZ::Module
    {
    public:
        AZ_RTTI(RadianceCascadeModule, "{B1C2D3E4-F5A6-7890-BCDE-F01234567890}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RadianceCascadeModule, AZ::SystemAllocator);

        RadianceCascadeModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                RadianceCascadeSystemComponent::CreateDescriptor(),
                RadianceCascadeComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{ azrtti_typeid<RadianceCascadeSystemComponent>() };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(Gem_RadianceCascade, RadianceCascade::RadianceCascadeModule)