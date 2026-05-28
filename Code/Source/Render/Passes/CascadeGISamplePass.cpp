#include <RadianceCascade/Render/Passes/CascadeGISamplePass.h>
#include <AzCore/Debug/Trace.h>

namespace RadianceCascade
{
    CascadeGISamplePass::CascadeGISamplePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor) {}

    AZ::RPI::Ptr<CascadeGISamplePass> CascadeGISamplePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeGISamplePass(descriptor);
    }

    void CascadeGISamplePass::BuildInternal()
    {
        ComputePass::BuildInternal();
    }

    void CascadeGISamplePass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        AZ_Printf("RadianceCascade", "CascadeGISamplePass is running!\n");
        ComputePass::FrameBeginInternal(params);
    }
}