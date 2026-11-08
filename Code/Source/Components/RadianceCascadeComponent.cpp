#include "RadianceCascadeComponent.h"
#include <RadianceCascade/RadianceCascadeFeatureProcessorInterface.h>
#include <Atom/RPI.Public/Scene.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace RadianceCascade
{
    void RadianceCascadeComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeComponent, AZ::Component>()->Version(2);
            if (auto* edit = serialize->GetEditContext())
            {
                edit->Class<RadianceCascadeComponent>("Radiance Cascade", "Enables Traditional Radiance Cascades for this scene.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"));
            }
        }
    }

    void RadianceCascadeComponent::Activate()
    {
        auto* scene = AZ::RPI::Scene::GetSceneForEntityId(GetEntityId());
        if (scene)
        {
            if (auto* fp = scene->GetFeatureProcessor<RadianceCascadeFeatureProcessorInterface>())
            {
                AZ_Printf("RadianceCascade", "Component Activate: Enabling Feature Processor.");
                fp->Enable();
            }
            else
            {
                AZ_Error("RadianceCascade", false, "Component Activate: Failed to find RadianceCascadeFeatureProcessor in the scene!");
            }
        }
        else
        {
            AZ_Error("RadianceCascade", false, "Component Activate: Failed to get RPI Scene!");
        }
    }

    void RadianceCascadeComponent::Deactivate()
    {
        auto* scene = AZ::RPI::Scene::GetSceneForEntityId(GetEntityId());
        if (scene)
        {
            if (auto* fp = scene->GetFeatureProcessor<RadianceCascadeFeatureProcessorInterface>())
            {
                AZ_Printf("RadianceCascade", "Component Deactivate: Disabling Feature Processor.");
                fp->Disable();
            }
        }
    }
}