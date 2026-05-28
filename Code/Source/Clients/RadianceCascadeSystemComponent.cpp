#include "RadianceCascadeSystemComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <Atom/RPI.Public/FeatureProcessorFactory.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>

#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <RadianceCascade/Render/Passes/CascadeMergePass.h>
#include <RadianceCascade/Render/Passes/CascadeGISamplePass.h>
#include <RadianceCascade/Render/Passes/CascadeReflectionSamplePass.h>

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

    void RadianceCascadeSystemComponent::LoadPassTemplateMappings()
    {
        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        if (passSystem)
            passSystem->LoadPassTemplateMappings("Passes/PassTemplates.azasset");
    }

    void RadianceCascadeSystemComponent::Activate()
    {
        // 1. Register the feature processor with the factory.
        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessor<CascadeFeatureProcessor>();

        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        if (!passSystem)
            return;

        // 2. Load pass template mappings once the pass system is ready.
        m_loadTemplatesHandler = AZ::RPI::PassSystemInterface::OnReadyLoadTemplatesEvent::Handler(
            [this]() { this->LoadPassTemplateMappings(); });
        passSystem->ConnectEvent(m_loadTemplatesHandler);

        // 3. Register our custom pass creators.
        passSystem->AddPassCreator(AZ::Name("CascadeInjectPass"),            &CascadeInjectPass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeMergePass"),             &CascadeMergePass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeGISamplePass"),          &CascadeGISamplePass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeReflectionSamplePass"), &CascadeReflectionSamplePass::Create);
    }

    void RadianceCascadeSystemComponent::Deactivate()
    {
        m_loadTemplatesHandler.Disconnect();
        AZ::RPI::FeatureProcessorFactory::Get()->UnregisterFeatureProcessor<CascadeFeatureProcessor>();
    }
}