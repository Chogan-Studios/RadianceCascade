#include <RadianceCascade/Render/Passes/CascadeReflectionSamplePass.h>

namespace RadianceCascade
{
    CascadeReflectionSamplePass::CascadeReflectionSamplePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    { }

    AZ::RPI::Ptr<CascadeReflectionSamplePass> CascadeReflectionSamplePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeReflectionSamplePass(descriptor);
    }
}