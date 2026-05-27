#include "RadianceCascadeModuleInterface.h"
#include "Passes/CascadeInjectPass.h"
#include "Passes/CascadeMergePass.h"
#include "Passes/CascadeSamplePass.h"
#include "CascadeFeatureProcessor.h"
#include <Atom/RPI.Public/RPISystemInterface.h>
#include <Atom/RPI.Public/Scene.h>

namespace RadianceCascade
{
    RadianceCascadeModuleInterface::RadianceCascadeModuleInterface()
    {
        // Could be empty
    }

    AZ::ComponentTypeList RadianceCascadeModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{}; // or your system component
    }

    void RadianceCascadeModuleInterface::Activate()
    {
        RegisterPasses();
        RegisterFeatureProcessors();
    }

    void RadianceCascadeModuleInterface::Deactivate()
    {
        // Pass system auto-cleans up pass creators; no explicit removal needed.
    }

    void RadianceCascadeModuleInterface::RegisterPasses()
    {
        auto* passSystem = AZ::RPI::RPISystemInterface::Get()->GetPassSystem();
        // Register the inject pass
        passSystem->AddPassCreator(Name("CascadeInjectPass"), &CascadeInjectPass::Create);
        passSystem->AddPassCreator(Name("CascadeMergePass"), &CascadeMergePass::Create);
        passSystem->AddPassCreator(Name("CascadeGISamplePass"), &CascadeGISamplePass::Create);
        // etc.
    }

    void RadianceCascadeModuleInterface::RegisterFeatureProcessors()
    {
        auto* scene = AZ::RPI::RPISystemInterface::Get()->GetDefaultScene();
        if (!scene) return;
        // Create our feature processor and add to scene
        auto fp = CascadeFeatureProcessor::Create();
        scene->AddFeatureProcessor(fp);
    }
}