#include <RadianceCascade/Render/Passes/CascadeMergePass.h>

namespace RadianceCascade
{
    CascadeMergePass::CascadeMergePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    { }

    AZ::RPI::Ptr<CascadeMergePass> CascadeMergePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeMergePass(descriptor);
    }
}