#pragma once
#include <AzCore/Component/Component.h>

namespace RadianceCascade
{
    class RadianceCascadeComponentConfig final : public AZ::ComponentConfig
    {
    public:
        AZ_RTTI(RadianceCascadeComponentConfig, "{F7A8B9C0-1D2E-3F4A-5B6C-7D8E9F0A1B2C}", AZ::ComponentConfig);
        AZ_CLASS_ALLOCATOR(RadianceCascadeComponentConfig, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);
    };
}