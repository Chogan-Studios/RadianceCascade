#pragma once
#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace RadianceCascade
{
    class CascadeMergePass final : public AZ::RPI::ComputePass
    {
    public:
        AZ_RTTI(CascadeMergePass, "{B2A3C4D5-E6F7-8901-2345-6789ABCDEF01}", AZ::RPI::ComputePass);
        AZ_CLASS_ALLOCATOR(CascadeMergePass, AZ::SystemAllocator);
        static AZ::RPI::Ptr<CascadeMergePass> Create(const AZ::RPI::PassDescriptor& descriptor);

    protected:
        CascadeMergePass(const AZ::RPI::PassDescriptor& descriptor);
        ~CascadeMergePass() = default;

        void BuildInternal() override;
    };
}