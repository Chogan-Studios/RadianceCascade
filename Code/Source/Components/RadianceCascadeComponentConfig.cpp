#include "RadianceCascadeComponentConfig.h"
#include <AzCore/Serialization/SerializeContext.h>

namespace RadianceCascade
{
    void RadianceCascadeComponentConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeComponentConfig, AZ::ComponentConfig>()
                ->Version(1);
        }
    }
}