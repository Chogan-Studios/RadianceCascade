#pragma once
#include <Components/RadianceCascadeComponent.h>
#include <AzToolsFramework/ToolsComponents/EditorComponentAdapter.h>
#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>

namespace RadianceCascade
{
    class EditorRadianceCascadeComponent final
        : public AzToolsFramework::Components::EditorComponentAdapter<RadianceCascadeComponentController, RadianceCascadeComponent, RadianceCascadeComponentConfig>
    {
    public:
        AZ_EDITOR_COMPONENT(EditorRadianceCascadeComponent, "{E1F2A3B4-C5D6-7E8F-9A0B-1C2D3E4F5A6B}", EditorComponentBase);

        using BaseClass = AzToolsFramework::Components::EditorComponentAdapter<RadianceCascadeComponentController, RadianceCascadeComponent, RadianceCascadeComponentConfig>;

        static void Reflect(AZ::ReflectContext* context);

        EditorRadianceCascadeComponent();
        explicit EditorRadianceCascadeComponent(const RadianceCascadeComponentConfig& config);
        ~EditorRadianceCascadeComponent() override = default;

        void Activate() override;
        void Deactivate() override;
    };
}