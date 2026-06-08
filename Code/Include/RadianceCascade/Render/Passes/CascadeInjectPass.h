#pragma once

#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace RadianceCascade
{
    class CascadeInjectPass final : public AZ::RPI::ComputePass
    {
    public:
        AZ_RTTI(CascadeInjectPass, "{DEADBEEF-CAFE-FACE-1234-567890ABCDEF}", AZ::RPI::ComputePass);
        AZ_CLASS_ALLOCATOR(CascadeInjectPass, AZ::SystemAllocator);

        static AZ::RPI::Ptr<CascadeInjectPass> Create(const AZ::RPI::PassDescriptor& descriptor);

    protected:
        CascadeInjectPass(const AZ::RPI::PassDescriptor& descriptor);
        ~CascadeInjectPass() = default;

        void SetupFrameGraphDependencies(AZ::RHI::FrameGraphInterface frameGraph) override;
        void BuildInternal() override;
        void FrameBeginInternal(FramePrepareParams params) override;
    };
}