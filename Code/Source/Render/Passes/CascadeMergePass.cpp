#include "RadianceCascade/Render/Passes/CascadeMergePass.h"

namespace RadianceCascade
{
    AZ::RPI::Ptr<CascadeMergePass> CascadeMergePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeMergePass(descriptor);
    }
}