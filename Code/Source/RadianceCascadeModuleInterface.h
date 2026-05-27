#pragma once

#include <AzCore/Module/Module.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>

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
        void RegisterPasses();
        void RegisterFeatureProcessors();
    };
}