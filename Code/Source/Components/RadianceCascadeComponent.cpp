#include "RadianceCascadeComponent.h"
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace RadianceCascade
{
    void RadianceCascadeComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeComponent, AZ::Component>()
                ->Version(1)
                ->Field("ProbeSpacing", &RadianceCascadeComponent::m_probeSpacing)
                ->Field("VolumeSize", &RadianceCascadeComponent::m_volumeSize);
        }

        if (auto* behavior = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behavior->ConstantProperty("RadianceCascadeComponentTypeId", BehaviorConstant(AZ::Uuid("{YOUR-COMP-GUID}")))
                ->Attribute(AZ::Script::Attributes::Module, "render")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common);
        }
    }

    void RadianceCascadeComponent::Activate()
    {
        // Later: register volume with feature processor
    }

    void RadianceCascadeComponent::Deactivate()
    {
    }
}