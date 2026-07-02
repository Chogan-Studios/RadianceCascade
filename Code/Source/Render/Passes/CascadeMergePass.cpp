#include <RadianceCascade/Render/Passes/CascadeMergePass.h>
#include <Atom/RPI.Public/Shader/ShaderResourceGroup.h>
#include <Atom/RPI.Public/Scene.h>
#include <AzCore/Name/Name.h>

namespace RadianceCascade
{
    AZ::RPI::Ptr<CascadeMergePass> CascadeMergePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeMergePass(descriptor);
    }

    CascadeMergePass::CascadeMergePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    {
    }

    // NOTE: This pass class is currently unused; merge passes are handled by CascadeInjectPass.
    // The following functions are kept for future use but the static helper GetMergeLevelsFromName
    // has been removed to avoid duplication with CascadeInjectPass.cpp.
    // If you later decide to use this class, define GetMergeLevelsFromName here.

    void CascadeMergePass::SetupFrameGraphDependencies(AZ::RHI::FrameGraphInterface frameGraph)
    {
        // This code is not called because the pass is not instantiated.
        ComputePass::SetupFrameGraphDependencies(frameGraph);
    }

    void CascadeMergePass::BuildInternal()
    {
        // This code is not called because the pass is not instantiated.
        ComputePass::BuildInternal();
    }

    void CascadeMergePass::FrameBeginInternal(FramePrepareParams params)
    {
        ComputePass::FrameBeginInternal(params);
    }
}