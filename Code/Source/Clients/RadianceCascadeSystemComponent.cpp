#include "RadianceCascadeSystemComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Debug/Trace.h>
#include <Atom/RPI.Public/FeatureProcessorFactory.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>

namespace RadianceCascade
{
    void RadianceCascadeSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeSystemComponent, AZ::Component>()
                ->Version(0);
        }
        CascadeFeatureProcessor::Reflect(context);
    }

    void RadianceCascadeSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("RadianceCascadeService"));
    }
    void RadianceCascadeSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("RadianceCascadeService"));
    }
    void RadianceCascadeSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("RPISystem"));
    }

    void RadianceCascadeSystemComponent::Activate()
    {
        AZ_Printf("RadianceCascade", "SystemComponent Activate!\n");

        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessor<CascadeFeatureProcessor>();

        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        if (passSystem)
        {
            // Register the one custom pass class – used by both the green diagnostic and the probe injection
            passSystem->AddPassCreator(AZ::Name("CascadeInjectPass"), &CascadeInjectPass::Create);

            // Load both templates (green diagnostic and probe injection)
            passSystem->LoadPassTemplateMappings("Passes/PassTemplates.azasset");
            AZ_Printf("RadianceCascade", "Pass creator registered and templates loaded.\n");
        }
    }

    void RadianceCascadeSystemComponent::Deactivate()
    {
        AZ::RPI::FeatureProcessorFactory::Get()->UnregisterFeatureProcessor<CascadeFeatureProcessor>();
    }
}