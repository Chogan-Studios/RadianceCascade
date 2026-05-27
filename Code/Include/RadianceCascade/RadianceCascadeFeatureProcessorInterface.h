/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/base.h>
#include <Atom/RPI.Public/FeatureProcessor.h>

namespace RadianceCascade
{
    class RadianceCascade;

    using RadianceCascadeHandle = AZStd::shared_ptr<RadianceCascade>;

    // RadianceCascadeFeatureProcessorInterface provides an interface to the feature processor for code outside of Atom
    class RadianceCascadeFeatureProcessorInterface
        : public AZ::RPI::FeatureProcessor
    {
    public:
        AZ_RTTI(RadianceCascadeFeatureProcessorInterface, "{4CDF671E-8BC4-430E-B1CE-E006FC8C1876}", AZ::RPI::FeatureProcessor);

    };
}
