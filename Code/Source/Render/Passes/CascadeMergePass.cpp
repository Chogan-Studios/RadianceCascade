#include <RadianceCascade/Render/Passes/CascadeMergePass.h>
#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <Atom/RPI.Public/Scene.h>

namespace RadianceCascade
{
    AZ::RPI::Ptr<CascadeMergePass> CascadeMergePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeMergePass(descriptor);
    }

    CascadeMergePass::CascadeMergePass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    {}

    void CascadeMergePass::BuildInternal()
    {
        ComputePass::BuildInternal();

        auto* scene = GetScene();
        if (!scene) return;
        auto* fp = scene->GetFeatureProcessor<CascadeFeatureProcessor>();
        if (!fp) return;

        uint32_t fineLevel = 0, coarseLevel = 1;
        if (GetName().GetStringView().ends_with("8_4")) { fineLevel = 1; coarseLevel = 2; }

        auto fineImage = fp->GetRadianceImage(fineLevel);
        if (fineImage) AttachImageToSlot(AZ::Name("FineRadiance"), fineImage);

        auto coarseImage = fp->GetRadianceImage(coarseLevel);
        if (coarseImage) AttachImageToSlot(AZ::Name("CoarseRadiance"), coarseImage);
    }
}