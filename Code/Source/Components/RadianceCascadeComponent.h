#pragma once
#include <AzCore/Component/Component.h>
#include <Components/RadianceCascadeComponentController.h>
#include <Components/RadianceCascadeComponentConfig.h>

namespace RadianceCascade
{
    class RadianceCascadeComponent final : public AZ::Component
    {
    public:
        AZ_COMPONENT(RadianceCascadeComponent, "{F6A1B2C3-4D5E-6F7A-8B9C-0D1E2F3A4B5C}");

        RadianceCascadeComponent() = default;
        explicit RadianceCascadeComponent(const RadianceCascadeComponentConfig& config);

        static void Reflect(AZ::ReflectContext* context);
        void Activate() override;
        void Deactivate() override;

    private:
        RadianceCascadeComponentController m_controller;
        RadianceCascadeComponentConfig m_configuration;
    };
}