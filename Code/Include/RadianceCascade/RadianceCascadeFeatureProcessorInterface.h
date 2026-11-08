#pragma once
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/base.h>
#include <AzCore/Math/Transform.h>
#include <Atom/RPI.Public/FeatureProcessor.h>
#include <Atom/RPI.Public/Image/AttachmentImage.h>
#include <RadianceCascade/Render/CascadeCommon.h>

namespace RadianceCascade
{
    class RadianceCascadeFeatureProcessorInterface : public AZ::RPI::FeatureProcessor
    {
    public:
        AZ_RTTI(RadianceCascadeFeatureProcessorInterface, "{4CDF671E-8BC4-430E-B1CE-E006FC8C1876}", AZ::RPI::FeatureProcessor);
        virtual ~RadianceCascadeFeatureProcessorInterface() = default;

        virtual void Enable() = 0;
        virtual void Disable() = 0;
        
        virtual const CascadeLevel& GetLevel(uint32_t i) const = 0;
        virtual AZ::Data::Instance<AZ::RPI::AttachmentImage> GetRadianceImage(uint32_t i) const = 0;
    };
}