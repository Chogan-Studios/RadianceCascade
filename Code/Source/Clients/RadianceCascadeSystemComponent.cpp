#include "RadianceCascadeSystemComponent.h"
#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <RadianceCascade/Render/Passes/CascadeRayMarchPass.h>
#include <RadianceCascade/Render/Passes/CascadeMergePass.h>
#include <Atom/RPI.Public/FeatureProcessorFactory.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace RadianceCascade
{
    void RadianceCascadeSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RadianceCascadeSystemComponent, AZ::Component>()->Version(1);
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
        // Ensure RPI system is ready before we use PassSystemInterface
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
        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessor<CascadeFeatureProcessor>();

        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        if (passSystem)
        {
            passSystem->AddPassCreator(AZ::Name("CascadeRayMarchPass"), &CascadeRayMarchPass::Create);
            passSystem->AddPassCreator(AZ::Name("CascadeMergePass"), &CascadeMergePass::Create);
            passSystem->LoadPassTemplateMappings("Passes/PassTemplates.azasset");
        }
        
        AZ_Printf("RadianceCascade", "Traditional Radiance Cascades Gem activated.");
    }

    void RadianceCascadeSystemComponent::Deactivate()
    {
        AZ::RPI::FeatureProcessorFactory::Get()->UnregisterFeatureProcessor<CascadeFeatureProcessor>();
    }
}