#include "EditorRadianceCascadeComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace RadianceCascade
{
    void EditorRadianceCascadeComponent::Reflect(AZ::ReflectContext* context)
    {
        RadianceCascadeComponentController::Reflect(context);   // register controller
        BaseClass::Reflect(context);

        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<EditorRadianceCascadeComponent, BaseClass>()
                ->Version(1);

            if (auto* edit = serialize->GetEditContext())
            {
                edit->Class<EditorRadianceCascadeComponent>("Radiance Cascade", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Rendering")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"));
            }
        }
    }

    EditorRadianceCascadeComponent::EditorRadianceCascadeComponent() = default;

    EditorRadianceCascadeComponent::EditorRadianceCascadeComponent(const RadianceCascadeComponentConfig& config)
        : BaseClass(config)
    {
    }

    void EditorRadianceCascadeComponent::Activate()
    {
        BaseClass::Activate();
    }

    void EditorRadianceCascadeComponent::Deactivate()
    {
        BaseClass::Deactivate();
    }
}