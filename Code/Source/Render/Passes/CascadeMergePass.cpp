#include <RadianceCascade/Render/Passes/CascadeMergePass.h>
#include <AzCore/Debug/Trace.h>

namespace RadianceCascade
{
    CascadeMergePass::CascadeMergePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor) {}

    AZ::RPI::Ptr<CascadeMergePass> CascadeMergePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeMergePass(descriptor);
    }

    void CascadeMergePass::BuildInternal()
    {
        ComputePass::BuildInternal();
    }

    void CascadeMergePass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        AZ_Printf("RadianceCascade", "CascadeMergePass is running!\n");
        ComputePass::FrameBeginInternal(params);
    }
}