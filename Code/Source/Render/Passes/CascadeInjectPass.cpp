#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>

namespace RadianceCascade
{
    CascadeInjectPass::CascadeInjectPass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    { }

    AZ::RPI::Ptr<CascadeInjectPass> CascadeInjectPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeInjectPass(descriptor);
    }

    void CascadeInjectPass::BuildInternal()
    {
        ComputePass::BuildInternal();
    }

    void CascadeInjectPass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        // TODO: implement mode switch when shader options are working
        ComputePass::FrameBeginInternal(params);
    }
}