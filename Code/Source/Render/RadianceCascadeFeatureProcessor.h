#pragma once
#include <AzCore/Module/Module.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <Atom/RPI.Public/FeatureProcessorFactory.h>

namespace RadianceCascade
{
    class RadianceCascadeModuleInterface : public AZ::Module
    {
    public:
        AZ_RTTI(RadianceCascadeModuleInterface, "{YOUR-MODULE-GUID}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RadianceCascadeModuleInterface, AZ::SystemAllocator);

        RadianceCascadeModuleInterface();
        ~RadianceCascadeModuleInterface() = default;

        AZ::ComponentTypeList GetRequiredSystemComponents() const override;

    protected:
        void Activate() override;
        void Deactivate() override;

    private:
        // Register pass classes (Inject, Merge, Sample) with the pass system
        void RegisterPasses();
        // Register the feature processor with the scene
        void RegisterFeatureProcessors();
    };
}