#pragma once

#include <AzCore/Component/Component.h>
#include <Components/RadianceCascadeComponentController.h>
#include <AzCore/Math/Vector3.h>

namespace RadianceCascade
{
    class RadianceCascadeComponent final : public AZ::Component
    {
    public:
        AZ_COMPONENT(RadianceCascadeComponent, "{YOUR-COMP-GUID}");

        static void Reflect(AZ::ReflectContext* context);
        void Activate() override;
        void Deactivate() override;

    private:
        RadianceCascadeComponentController m_controller;
        // Volume settings (will be extended later)
        float m_probeSpacing = 1.0f;
        AZ::Vector3 m_volumeSize = AZ::Vector3(20.0f);
    };
}