#include "RadianceCascadeComponentController.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace RadianceCascade
{
    RadianceCascadeComponentController::RadianceCascadeComponentController(const RadianceCascadeComponentConfig& config)
        : m_configuration(config)
    {
    }

    void RadianceCascadeComponentController::Reflect(AZ::ReflectContext* context)
    {
        RadianceCascadeComponentConfig::Reflect(context);

        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeComponentController>()
                ->Version(1)
                ->Field("Configuration", &RadianceCascadeComponentController::m_configuration);

            if (auto* edit = serialize->GetEditContext())
            {
                edit->Class<RadianceCascadeComponentController>("RadianceCascade Controller", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RadianceCascadeComponentController::m_configuration, "Configuration", "")
                    ;
            }
        }
    }
}