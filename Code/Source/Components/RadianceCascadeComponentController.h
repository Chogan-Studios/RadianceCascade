/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>

#include <RadianceCascade/RadianceCascadeFeatureProcessorInterface.h>

namespace RadianceCascade
{
    class RadianceCascadeComponentConfig final
        : public AZ::ComponentConfig
    {
    public:
        AZ_RTTI(RadianceCascadeComponentConfig, "{22FA459F-C5A2-42E1-9EA0-DAC9681FE254}", ComponentConfig);
        AZ_CLASS_ALLOCATOR(RadianceCascadeComponentConfig, AZ::SystemAllocator);
        static void Reflect(AZ::ReflectContext* context);

        RadianceCascadeComponentConfig() = default;

        AZ::u64 m_entityId{ AZ::EntityId::InvalidEntityId };
    };

    class RadianceCascadeComponentController final
        : public AZ::Data::AssetBus::MultiHandler
        , private AZ::TransformNotificationBus::Handler
    {
    public:
        friend class EditorRadianceCascadeComponent;

        AZ_RTTI(RadianceCascadeComponentController, "{EE853CDE-3DFA-4424-B3C5-B2117462A8DC}");
        AZ_CLASS_ALLOCATOR(RadianceCascadeComponentController, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        RadianceCascadeComponentController() = default;
        RadianceCascadeComponentController(const RadianceCascadeComponentConfig& config);

        void Activate(AZ::EntityId entityId);
        void Deactivate();
        void SetConfiguration(const RadianceCascadeComponentConfig& config);
        const RadianceCascadeComponentConfig& GetConfiguration() const;

    private:

        AZ_DISABLE_COPY(RadianceCascadeComponentController);

        // TransformNotificationBus overrides
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        // handle for this probe in the feature processor
        RadianceCascadeHandle m_handle;

        RadianceCascadeFeatureProcessorInterface* m_featureProcessor = nullptr;
        AZ::TransformInterface* m_transformInterface = nullptr;
        AZ::EntityId m_entityId;
        
        RadianceCascadeComponentConfig m_configuration;

    };
}
