#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <Atom/RPI.Public/Scene.h>

namespace RadianceCascade
{
    AZ::RPI::Ptr<CascadeInjectPass> CascadeInjectPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeInjectPass(descriptor);
    }

    CascadeInjectPass::CascadeInjectPass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    { }

    void CascadeInjectPass::SetupFrameGraphDependencies(AZ::RHI::FrameGraphInterface frameGraph)
    {
        ComputePass::SetupFrameGraphDependencies(frameGraph);
    }

    void CascadeInjectPass::BuildInternal()
    {
        ComputePass::BuildInternal();
    }

    void CascadeInjectPass::FrameBeginInternal(FramePrepareParams params)
    {
        ComputePass::FrameBeginInternal(params);
    }
}