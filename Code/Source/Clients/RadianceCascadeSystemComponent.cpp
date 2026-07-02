#include "RadianceCascadeSystemComponent.h"
#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <Atom/RPI.Public/FeatureProcessorFactory.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace RadianceCascade
{
    void RadianceCascadeSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeSystemComponent, AZ::Component>()
                ->Version(0);

            // Regression 2: Reflect the feature processor
            CascadeFeatureProcessor::Reflect(context);
        }
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
        // Regression 3: Ensure RPI system is ready before we use PassSystemInterface
        required.push_back(AZ_CRC_CE("RPISystem"));
    }

    void RadianceCascadeSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void RadianceCascadeSystemComponent::Init()
    {
    }

    void RadianceCascadeSystemComponent::Activate()
    {
        // Register the feature processor
        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessor<CascadeFeatureProcessor>();

        // Regression 1: Register the pass creator and load the template mappings
        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        if (passSystem)
        {
            passSystem->AddPassCreator(AZ::Name("CascadeInjectPass"), &CascadeInjectPass::Create);
            passSystem->LoadPassTemplateMappings("Passes/PassTemplates.azasset");
        }

        AZ_Printf("RadianceCascade", "Radiance Cascade Gem activated.");
    }

    void RadianceCascadeSystemComponent::Deactivate()
    {
        // Optional: unregister if needed
        AZ::RPI::FeatureProcessorFactory::Get()->UnregisterFeatureProcessor<CascadeFeatureProcessor>();
    }
}