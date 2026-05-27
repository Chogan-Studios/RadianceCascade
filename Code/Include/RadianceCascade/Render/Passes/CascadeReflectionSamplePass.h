#pragma once
#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace RadianceCascade
{
    class CascadeReflectionSamplePass : public AZ::RPI::ComputePass
    {
    public:
        AZ_RTTI(CascadeReflectionSamplePass, "{D4E5F6A7-8901-BCDE-F234-567890ABCDEF}", AZ::RPI::ComputePass);
        AZ_CLASS_ALLOCATOR(CascadeReflectionSamplePass, AZ::SystemAllocator);

        explicit CascadeReflectionSamplePass(const AZ::RPI::PassDescriptor& descriptor);
        static AZ::RPI::Ptr<CascadeReflectionSamplePass> Create(const AZ::RPI::PassDescriptor& descriptor);
    };
}