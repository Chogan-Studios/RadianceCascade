#include <AzCore/Module/Module.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <Clients/RadianceCascadeSystemComponent.h>

namespace RadianceCascade
{
    class RadianceCascadeModule : public AZ::Module
    {
    public:
        AZ_RTTI(RadianceCascadeModule, "{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RadianceCascadeModule, AZ::SystemAllocator);

        RadianceCascadeModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                RadianceCascadeSystemComponent::CreateDescriptor(),
            });
        }
    };
}

AZ_DECLARE_MODULE_CLASS(Gem_RadianceCascade, RadianceCascade::RadianceCascadeModule)