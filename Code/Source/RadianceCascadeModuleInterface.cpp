#include "RadianceCascadeModuleInterface.h"
#include "Render/CascadeFeatureProcessor.h"
#include "Render/Passes/CascadeInjectPass.h"
#include "Render/Passes/CascadeMergePass.h"
#include "Render/Passes/CascadeGISamplePass.h"
#include "Render/Passes/CascadeReflectionSamplePass.h"
#include <Atom/RPI.Public/RPISystemInterface.h>
#include <Atom/RPI.Public/Scene.h>

namespace RadianceCascade
{
    RadianceCascadeModuleInterface::RadianceCascadeModuleInterface()
    {
    }

    AZ::ComponentTypeList RadianceCascadeModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{};
    }

    void RadianceCascadeModuleInterface::Activate()
    {
        RegisterPasses();
        RegisterFeatureProcessors();
    }

    void RadianceCascadeModuleInterface::Deactivate()
    {
    }

    void RadianceCascadeModuleInterface::RegisterPasses()
    {
        auto* passSystem = AZ::RPI::RPISystemInterface::Get()->GetPassSystem();
        passSystem->AddPassCreator(AZ::Name("CascadeInjectPass"), &CascadeInjectPass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeMergePass"), &CascadeMergePass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeGISamplePass"), &CascadeGISamplePass::Create);
        passSystem->AddPassCreator(AZ::Name("CascadeReflectionSamplePass"), &CascadeReflectionSamplePass::Create);
    }

    void RadianceCascadeModuleInterface::RegisterFeatureProcessors()
    {
        auto* scene = AZ::RPI::RPISystemInterface::Get()->GetDefaultScene();
        if (scene)
        {
            auto fp = CascadeFeatureProcessor::Create();
            scene->AddFeatureProcessor(AZStd::move(fp));
        }
    }
}