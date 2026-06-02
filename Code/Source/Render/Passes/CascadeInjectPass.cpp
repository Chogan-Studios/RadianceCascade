#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <AzCore/Debug/Trace.h>

namespace RadianceCascade
{
    CascadeInjectPass::CascadeInjectPass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    {
        AZ_Error("RadianceCascade", true, "CascadeInjectPass CONSTRUCTED!\n");
    }

    AZ::RPI::Ptr<CascadeInjectPass> CascadeInjectPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeInjectPass(descriptor);
    }

    void CascadeInjectPass::BuildInternal()
    {
        ComputePass::BuildInternal();
        AZ_Error("RadianceCascade", true, "CascadeInjectPass BUILD!\n");
    }

    void CascadeInjectPass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        AZ_Error("RadianceCascade", true, "CascadeInjectPass is running!\n");
        ComputePass::FrameBeginInternal(params);
    }
}