#pragma once
#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace RadianceCascade
{
    class CascadeInjectPass : public AZ::RPI::ComputePass
    {
    public:
        AZ_RTTI(CascadeInjectPass, "{F1A2B3C4-5678-90AB-CDEF-1234567890AB}", AZ::RPI::ComputePass);
        AZ_CLASS_ALLOCATOR(CascadeInjectPass, AZ::SystemAllocator);

        static AZ::RPI::Ptr<CascadeInjectPass> Create(const AZ::RPI::PassDescriptor& descriptor);

    protected:
        void BuildInternal() override;
        void FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params) override;

    private:
        AZ::RPI::ShaderOptionValue m_modeOption;
    };
}