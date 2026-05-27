#include "RadianceCascadeSystemComponent.h"
#include <AzCore/Serialization/SerializeContext.h>

#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <RadianceCascade/Render/Passes/CascadeMergePass.h>
#include <RadianceCascade/Render/Passes/CascadeGISamplePass.h>
#include <RadianceCascade/Render/Passes/CascadeReflectionSamplePass.h>

#include <RadianceCascade/Render/CascadeFeatureProcessor.h>

#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <Atom/RPI.Public/RPISystemInterface.h>
#include <Atom/RPI.Public/Scene.h>

namespace RadianceCascade
{
    void RadianceCascadeSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ_UNUSED(context);
    }

    void RadianceCascadeSystemComponent::Activate()
    {
        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        passSystem->AddPassCreator(AZ::Name("CascadeInjectPass"), &CascadeInjectPass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeMergePass"), &CascadeMergePass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeGISamplePass"), &CascadeGISamplePass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeReflectionSamplePass"), &CascadeReflectionSamplePass::Create);

        auto* rpiSystem = AZ::RPI::RPISystemInterface::Get();
        AZ::RPI::ScenePtr scene(rpiSystem->GetSceneByName(AZ::Name("Main")));
        if (scene)
        {
            scene->EnableFeatureProcessor<CascadeFeatureProcessor>();
        }
    }

    void RadianceCascadeSystemComponent::Deactivate()
    {
    }
}