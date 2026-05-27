/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Components/RadianceCascadeComponentController.h>

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Asset/AssetManagerBus.h>
#include <AzCore/Asset/AssetSerializer.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <AzFramework/Entity/EntityContextBus.h>
#include <AzFramework/Entity/EntityContext.h>
#include <AzFramework/Scene/Scene.h>
#include <AzFramework/Scene/SceneSystemInterface.h>

#include <AzCore/RTTI/BehaviorContext.h>

#include <Atom/RPI.Public/Scene.h>

namespace RadianceCascade
{
    void RadianceCascadeComponentConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RadianceCascadeComponentConfig>()
                ;
        }
    }

    void RadianceCascadeComponentController::Reflect(AZ::ReflectContext* context)
    {
        RadianceCascadeComponentConfig::Reflect(context);

        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RadianceCascadeComponentController>()
                ->Version(0)
                ->Field("Configuration", &RadianceCascadeComponentController::m_configuration);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<RadianceCascadeComponentController>(
                    "RadianceCascadeComponentController", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RadianceCascadeComponentController::m_configuration, "Configuration", "")
                        ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                    ;
            }
        }
    }

    void RadianceCascadeComponentController::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("TransformService"));
    }

    void RadianceCascadeComponentController::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("RadianceCascadeService"));
    }

    void RadianceCascadeComponentController::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("RadianceCascadeService"));
    }

    void RadianceCascadeComponentController::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("TransformService"));
    }

    RadianceCascadeComponentController::RadianceCascadeComponentController(const RadianceCascadeComponentConfig& config)
        : m_configuration(config)
    {
    }

    void RadianceCascadeComponentController::Activate(AZ::EntityId entityId)
    {
        m_entityId = entityId;

        AZ::TransformNotificationBus::Handler::BusConnect(m_entityId);

        m_featureProcessor = AZ::RPI::Scene::GetFeatureProcessorForEntity<RadianceCascadeFeatureProcessorInterface>(entityId);
        AZ_Assert(m_featureProcessor, "RadianceCascadeComponentController was unable to find a RadianceCascadeFeatureProcessor on the EntityContext provided.");

    }

    void RadianceCascadeComponentController::Deactivate()
    {
        AZ::TransformNotificationBus::Handler::BusDisconnect();
    }

    void RadianceCascadeComponentController::SetConfiguration(const RadianceCascadeComponentConfig& config)
    {
        m_configuration = config;
    }

    const RadianceCascadeComponentConfig& RadianceCascadeComponentController::GetConfiguration() const
    {
        return m_configuration;
    }

    void RadianceCascadeComponentController::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, [[maybe_unused]] const AZ::Transform& world)
    {
        if (!m_featureProcessor)
        {
            return;
        }
    }
}
