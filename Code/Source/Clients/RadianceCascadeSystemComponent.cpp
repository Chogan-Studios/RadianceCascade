#include "RadianceCascadeSystemComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Console/Console.h>

namespace RadianceCascade
{
    void RadianceCascadeSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        // ...
    }

    void RadianceCascadeSystemComponent::Activate()
    {
        // Register CVars if not already done via AZ_CVAR (they are already)
        RadianceCascadeRequestBus::Handler::BusConnect();
    }

    void RadianceCascadeSystemComponent::Deactivate()
    {
        RadianceCascadeRequestBus::Handler::BusDisconnect();
    }
}