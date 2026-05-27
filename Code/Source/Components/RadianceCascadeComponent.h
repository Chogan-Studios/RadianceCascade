/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Components/RadianceCascadeComponentController.h>
#include <AzFramework/Components/ComponentAdapter.h>

namespace RadianceCascade
{
    inline constexpr AZ::TypeId RadianceCascadeComponentTypeId { "{34868EAF-BA20-42D2-BBC5-A5A8D538C6F0}" };

    class RadianceCascadeComponent final
        : public AzFramework::Components::ComponentAdapter<RadianceCascadeComponentController, RadianceCascadeComponentConfig>
    {
    public:
        using BaseClass = AzFramework::Components::ComponentAdapter<RadianceCascadeComponentController, RadianceCascadeComponentConfig>;
        AZ_COMPONENT(RadianceCascadeComponent, RadianceCascadeComponentTypeId, BaseClass);

        RadianceCascadeComponent() = default;
        RadianceCascadeComponent(const RadianceCascadeComponentConfig& config);

        static void Reflect(AZ::ReflectContext* context);
    };
}
