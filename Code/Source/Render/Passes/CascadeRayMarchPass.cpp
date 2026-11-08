#include <RadianceCascade/Render/Passes/CascadeRayMarchPass.h>
#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <Atom/RPI.Public/Scene.h>

namespace RadianceCascade
{
    AZ::RPI::Ptr<CascadeRayMarchPass> CascadeRayMarchPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeRayMarchPass(descriptor);
    }

    CascadeRayMarchPass::CascadeRayMarchPass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    {}

    void CascadeRayMarchPass::BuildInternal()
    {
        ComputePass::BuildInternal();

        auto* scene = GetScene();
        if (!scene) return;
        auto* fp = scene->GetFeatureProcessor<CascadeFeatureProcessor>();
        if (!fp) return;

        // Attach the FP's radiance textures to this pass's slots
        uint32_t level = 0;
        if (GetName().GetStringView().ends_with("C1")) level = 1;
        else if (GetName().GetStringView().ends_with("C2")) level = 2;

        auto radianceImage = fp->GetRadianceImage(level);
        if (radianceImage)
            AttachImageToSlot(AZ::Name("Output"), radianceImage);
    }
}