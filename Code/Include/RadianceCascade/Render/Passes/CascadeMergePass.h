#pragma once
#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace RadianceCascade
{
    class CascadeMergePass : public AZ::RPI::ComputePass
    {
    public:
        AZ_RTTI(CascadeMergePass, "{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}", AZ::RPI::ComputePass);
        AZ_CLASS_ALLOCATOR(CascadeMergePass, AZ::SystemAllocator);

        explicit CascadeMergePass(const AZ::RPI::PassDescriptor& descriptor);
        static AZ::RPI::Ptr<CascadeMergePass> Create(const AZ::RPI::PassDescriptor& descriptor);
    };
}