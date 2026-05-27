#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/RTTI/ReflectContext.h>
#include <RadianceCascade/Render/CascadeCommon.h>   // for InjectionMode

namespace RadianceCascade
{
    class RadianceCascadeComponentConfig final : public AZ::ComponentConfig
    {
    public:
        AZ_RTTI(RadianceCascadeComponentConfig, "{F7A8B9C0-1D2E-3F4A-5B6C-7D8E9F0A1B2C}", AZ::ComponentConfig);
        AZ_CLASS_ALLOCATOR(RadianceCascadeComponentConfig, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);

        float m_probeSpacing = 1.0f;
        AZ::Vector3 m_volumeSize = AZ::Vector3(20.0f);
        InjectionMode m_injectionMode = InjectionMode::Software;
        float m_temporalWeight = 0.08f;
    };
}