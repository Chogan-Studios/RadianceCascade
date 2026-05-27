
#include <RadianceCascade/RadianceCascadeTypeIds.h>
#include <RadianceCascadeModuleInterface.h>
#include "RadianceCascadeSystemComponent.h"

#include <AzCore/RTTI/RTTI.h>

#include <Components/RadianceCascadeComponent.h>

namespace RadianceCascade
{
    class RadianceCascadeModule
        : public RadianceCascadeModuleInterface
    {
    public:
        AZ_RTTI(RadianceCascadeModule, RadianceCascadeModuleTypeId, RadianceCascadeModuleInterface);
        AZ_CLASS_ALLOCATOR(RadianceCascadeModule, AZ::SystemAllocator);

        RadianceCascadeModule()
        {
            m_descriptors.insert(m_descriptors.end(),
                {
                    RadianceCascadeSystemComponent::CreateDescriptor(),
                    RadianceCascadeComponent::CreateDescriptor(),
                });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const
        {
            return AZ::ComponentTypeList{ azrtti_typeid<RadianceCascadeSystemComponent>() };
        }
    };
}// namespace RadianceCascade

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), RadianceCascade::RadianceCascadeModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_RadianceCascade, RadianceCascade::RadianceCascadeModule)
#endif
