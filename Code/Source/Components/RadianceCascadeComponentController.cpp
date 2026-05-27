#include "RadianceCascadeComponentController.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace RadianceCascade
{
    RadianceCascadeComponentController::RadianceCascadeComponentController(const RadianceCascadeComponentConfig& config)
        : m_injectionMode(config.m_injectionMode)
        , m_probeSpacing(config.m_probeSpacing)
        , m_volumeSize(config.m_volumeSize)
        , m_temporalWeight(config.m_temporalWeight)
    {
    }

    void RadianceCascadeComponentController::SetConfiguration(const RadianceCascadeComponentConfig& config)
    {
        m_injectionMode = config.m_injectionMode;
        m_probeSpacing = config.m_probeSpacing;
        m_volumeSize = config.m_volumeSize;
        m_temporalWeight = config.m_temporalWeight;
    }

    RadianceCascadeComponentConfig RadianceCascadeComponentController::GetConfiguration() const
    {
        RadianceCascadeComponentConfig config;
        config.m_injectionMode = m_injectionMode;
        config.m_probeSpacing = m_probeSpacing;
        config.m_volumeSize = m_volumeSize;
        config.m_temporalWeight = m_temporalWeight;
        return config;
    }

    void RadianceCascadeComponentController::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeComponentController>()
                ->Version(1)
                ->Field("InjectionMode", &RadianceCascadeComponentController::m_injectionMode)
                ->Field("ProbeSpacing", &RadianceCascadeComponentController::m_probeSpacing)
                ->Field("VolumeSize", &RadianceCascadeComponentController::m_volumeSize)
                ->Field("TemporalWeight", &RadianceCascadeComponentController::m_temporalWeight);

            if (auto* edit = serialize->GetEditContext())
            {
                edit->Class<RadianceCascadeComponentController>("Radiance Cascade", "")
                    ->DataElement(AZ::Edit::UIHandlers::ComboBox, &RadianceCascadeComponentController::m_injectionMode,
                        "Injection Mode", "Software: fast rasterization, Hardware: ray traced")
                        ->EnumAttribute(InjectionMode::Software, "Software")
                        ->EnumAttribute(InjectionMode::HardwareRT, "Hardware RT")
                    ->DataElement(AZ::Edit::UIHandlers::Slider, &RadianceCascadeComponentController::m_probeSpacing,
                        "Probe Spacing", "Distance between probes")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Max, 10.0f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RadianceCascadeComponentController::m_volumeSize,
                        "Volume Size", "World size of the cascade")
                    ->DataElement(AZ::Edit::UIHandlers::Slider, &RadianceCascadeComponentController::m_temporalWeight,
                        "Temporal Blend", "Blend weight for probe history (lower = smoother)")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.01f)
                        ->Attribute(AZ::Edit::Attributes::Max, 1.0f);
            }
        }
    }
}