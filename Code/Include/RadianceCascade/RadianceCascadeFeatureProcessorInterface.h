#pragma once

#include <AzCore/base.h>
#include <Atom/RPI.Public/FeatureProcessor.h>
#include <Atom/RPI.Reflect/Image/Image.h>
#include <AzCore/std/containers/array.h>
#include <AzCore/Math/Transform.h>
#include <RadianceCascade/Render/CascadeCommon.h>
#include <Components/RadianceCascadeComponentConfig.h>

namespace RadianceCascade
{
    class RadianceCascadeFeatureProcessorInterface : public AZ::RPI::FeatureProcessor
    {
    public:
        AZ_RTTI(RadianceCascadeFeatureProcessorInterface, "{4CDF671E-8BC4-430E-B1CE-E006FC8C1876}", AZ::RPI::FeatureProcessor);

        virtual AZ::Data::Instance<AZ::RPI::Image> GetProbeSHBuffer(uint32_t cascadeLevel) const = 0;
        virtual AZ::Data::Instance<AZ::RPI::Image> GetProbeOctahedralMap() const = 0;
        virtual const AZStd::array<uint32_t, MaxCascadeLevels>& GetActiveProbeCounts() const = 0;
        virtual InjectionMode GetInjectionMode() const = 0;
        virtual void ResetAllProbes() = 0;
        virtual void SetCameraTransform(const AZ::Transform& worldTransform) = 0;

        // Apply a new configuration from the component
        virtual void SetConfiguration(const RadianceCascadeComponentConfig& config) = 0;
    };
}