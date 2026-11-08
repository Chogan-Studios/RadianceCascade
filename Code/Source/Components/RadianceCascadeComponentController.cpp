#include "RadianceCascadeComponentController.h"
#include <AzCore/Serialization/SerializeContext.h>

namespace RadianceCascade
{
    void RadianceCascadeComponentController::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            // Register the controller with the serialization context
            serialize->Class<RadianceCascadeComponentController>()
                ->Version(1);
        }
    }
}