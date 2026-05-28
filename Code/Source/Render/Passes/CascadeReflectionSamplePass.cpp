#include <RadianceCascade/Render/Passes/CascadeReflectionSamplePass.h>
#include <AzCore/Debug/Trace.h>

namespace RadianceCascade
{
    CascadeReflectionSamplePass::CascadeReflectionSamplePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor) {}

    AZ::RPI::Ptr<CascadeReflectionSamplePass> CascadeReflectionSamplePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeReflectionSamplePass(descriptor);
    }

    void CascadeReflectionSamplePass::BuildInternal()
    {
        ComputePass::BuildInternal();
    }

    void CascadeReflectionSamplePass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        AZ_Printf("RadianceCascade", "CascadeReflectionSamplePass is running!\n");
        ComputePass::FrameBeginInternal(params);
    }
}