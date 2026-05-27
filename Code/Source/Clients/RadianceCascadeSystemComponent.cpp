/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "RadianceCascadeSystemComponent.h"

#include <RadianceCascade/RadianceCascadeTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

#include <Atom/RPI.Public/FeatureProcessorFactory.h>

#include <Render/RadianceCascadeFeatureProcessor.h>

namespace RadianceCascade
{
    AZ_COMPONENT_IMPL(RadianceCascadeSystemComponent, "RadianceCascadeSystemComponent",
        RadianceCascadeSystemComponentTypeId);

    void RadianceCascadeSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RadianceCascadeSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }

        RadianceCascadeFeatureProcessor::Reflect(context);
    }

    void RadianceCascadeSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("RadianceCascadeSystemService"));
    }

    void RadianceCascadeSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("RadianceCascadeSystemService"));
    }

    void RadianceCascadeSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("RPISystem"));
    }

    void RadianceCascadeSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    RadianceCascadeSystemComponent::RadianceCascadeSystemComponent()
    {
        if (RadianceCascadeInterface::Get() == nullptr)
        {
            RadianceCascadeInterface::Register(this);
        }
    }

    RadianceCascadeSystemComponent::~RadianceCascadeSystemComponent()
    {
        if (RadianceCascadeInterface::Get() == this)
        {
            RadianceCascadeInterface::Unregister(this);
        }
    }

    void RadianceCascadeSystemComponent::Init()
    {
    }

    void RadianceCascadeSystemComponent::Activate()
    {
        // Register CVars
        AZ::RPI::CvarManager::Get()->RegisterCvar("r_radianceCascade/mode", 0);
        AZ::RPI::CvarManager::Get()->RegisterCvar("r_radianceCascade/temporalWeight", 0.08f);
        // ... more CVars
        RadianceCascadeRequestBus::Handler::BusConnect();

        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessor<RadianceCascadeFeatureProcessor>();
    }

    void RadianceCascadeSystemComponent::Deactivate()
    {
        AZ::RPI::FeatureProcessorFactory::Get()->UnregisterFeatureProcessor<RadianceCascadeFeatureProcessor>();

        RadianceCascadeRequestBus::Handler::BusDisconnect();
    }

} // namespace RadianceCascade
