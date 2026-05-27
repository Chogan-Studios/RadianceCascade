#include <RadianceCascade/Render/Passes/CascadeGISamplePass.h>

namespace RadianceCascade
{
    CascadeGISamplePass::CascadeGISamplePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    { }

    AZ::RPI::Ptr<CascadeGISamplePass> CascadeGISamplePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeGISamplePass(descriptor);
    }
}