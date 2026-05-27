#include "RadianceCascadeComponentConfig.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace RadianceCascade
{
    void RadianceCascadeComponentConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeComponentConfig, AZ::ComponentConfig>()
                ->Version(2)
                ->Field("ProbeSpacing", &RadianceCascadeComponentConfig::m_probeSpacing)
                ->Field("VolumeSize", &RadianceCascadeComponentConfig::m_volumeSize)
                ->Field("InjectionMode", &RadianceCascadeComponentConfig::m_injectionMode)
                ->Field("TemporalWeight", &RadianceCascadeComponentConfig::m_temporalWeight);
        }
    }
}