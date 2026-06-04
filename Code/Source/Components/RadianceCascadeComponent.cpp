#include "RadianceCascadeComponent.h"
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <Atom/RPI.Public/Scene.h>
#include <RadianceCascade/RadianceCascadeFeatureProcessorInterface.h>

namespace RadianceCascade
{
    RadianceCascadeComponent::RadianceCascadeComponent(const RadianceCascadeComponentConfig& config)
        : m_configuration(config)
    {
    }

    void RadianceCascadeComponent::Reflect(AZ::ReflectContext* context)
    {
        RadianceCascadeComponentConfig::Reflect(context);

        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeComponent, AZ::Component>()
                ->Version(1)
                ->Field("Configuration", &RadianceCascadeComponent::m_configuration);
        }

        if (auto* behavior = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behavior->ConstantProperty("RadianceCascadeComponentTypeId", BehaviorConstant(AZ::Uuid("{F6A1B2C3-4D5E-6F7A-8B9C-0D1E2F3A4B5C}")))
                ->Attribute(AZ::Script::Attributes::Module, "render")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common);
        }
    }

    void RadianceCascadeComponent::Activate()
    {
        auto* scene = AZ::RPI::Scene::GetSceneForEntityId(GetEntityId());
        if (scene)
        {
            auto* fp = scene->GetFeatureProcessor<RadianceCascadeFeatureProcessorInterface>();
            if (fp)
            {
                fp->SetConfiguration(m_configuration);
            }
        }
    }

    void RadianceCascadeComponent::Deactivate()
    {
    }
}