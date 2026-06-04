#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <AzCore/Debug/Trace.h>

namespace RadianceCascade
{
    CascadeInjectPass::CascadeInjectPass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    {
    }

    AZ::RPI::Ptr<CascadeInjectPass> CascadeInjectPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeInjectPass(descriptor);
    }

    void CascadeInjectPass::BuildInternal()
    {
        ComputePass::BuildInternal();
        AZ_Printf("RadianceCascade",
            "CascadeInjectPass BuildInternal: shader=%s srg=%s\n",
            GetShader() ? "loaded" : "MISSING",
            GetShaderResourceGroup() ? "ok" : "MISSING");
    }

    void CascadeInjectPass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        ComputePass::FrameBeginInternal(params);
    }
}