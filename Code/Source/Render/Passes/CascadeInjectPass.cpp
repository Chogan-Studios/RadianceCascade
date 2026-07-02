#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <RadianceCascade/Render/CascadeFeatureProcessor.h>
#include <Atom/RPI.Public/Shader/ShaderResourceGroup.h>
#include <Atom/RPI.Public/Scene.h>
#include <AzCore/Name/Name.h>

namespace RadianceCascade
{
    AZ::RPI::Ptr<CascadeInjectPass> CascadeInjectPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeInjectPass(descriptor);
    }

    CascadeInjectPass::CascadeInjectPass(const AZ::RPI::PassDescriptor& descriptor)
        : ComputePass(descriptor)
    {}

    // Fixed parser: finds "_C" and reads the digit after it
    static uint32_t GetCascadeLevelFromName(const AZ::Name& passName)
    {
        AZStd::string_view name = passName.GetStringView();
        size_t pos = name.find("_C");
        if (pos != AZStd::string_view::npos && pos + 2 < name.size())
        {
            char levelChar = name[pos + 2];
            if (levelChar >= '0' && levelChar <= '2')
                return static_cast<uint32_t>(levelChar - '0');
        }
        return 0;
    }

    static bool IsMergePass(const AZ::Name& passName)
    {
        return passName.GetStringView().starts_with("CascadeMerge");
    }

    static bool IsDiffuseGIPass(const AZ::Name& passName)
    {
        return passName == AZ::Name("CascadeDiffuseGIPass");
    }

    static bool IsResolveDepthPass(const AZ::Name& passName)
    {
        return passName == AZ::Name("CascadeResolveDepthPass");
    }

    static bool GetMergeLevelsFromName(const AZ::Name& passName, uint32_t& fineLevel, uint32_t& coarseLevel)
    {
        AZStd::string_view name = passName.GetStringView();
        if (name == "CascadeMerge_16to8")
        {
            fineLevel = 0; coarseLevel = 1;
            return true;
        }
        else if (name == "CascadeMerge_8to4")
        {
            fineLevel = 1; coarseLevel = 2;
            return true;
        }
        return false;
    }

    void CascadeInjectPass::SetupFrameGraphDependencies(AZ::RHI::FrameGraphInterface frameGraph)
    {
        auto* scene = GetScene();
        if (!scene) return;
        auto* fp = scene->GetFeatureProcessor<CascadeFeatureProcessor>();
        if (!fp) return;

        const AZ::Name& passName = GetName();

        if (IsResolveDepthPass(passName))
        {
            auto depthImage = fp->GetResolvedDepthImage();
            if (depthImage)
                frameGraph.GetAttachmentDatabase().ImportImage(depthImage->GetAttachmentId(), depthImage->GetRHIImage());
        }
        else if (IsDiffuseGIPass(passName))
        {
            auto probeImage = fp->GetProbeSHAttachment(0);
            if (probeImage)
                frameGraph.GetAttachmentDatabase().ImportImage(probeImage->GetAttachmentId(), probeImage->GetRHIImage());

            auto outputImage = fp->GetDiffuseGIOutput();
            if (outputImage)
                frameGraph.GetAttachmentDatabase().ImportImage(outputImage->GetAttachmentId(), outputImage->GetRHIImage());

            auto depthImage = fp->GetResolvedDepthImage();
            if (depthImage)
                frameGraph.GetAttachmentDatabase().ImportImage(depthImage->GetAttachmentId(), depthImage->GetRHIImage());
        }
        else if (IsMergePass(passName))
        {
            uint32_t fineLevel, coarseLevel;
            if (GetMergeLevelsFromName(passName, fineLevel, coarseLevel))
            {
                auto fineImage = fp->GetProbeSHAttachment(fineLevel);
                if (fineImage)
                    frameGraph.GetAttachmentDatabase().ImportImage(fineImage->GetAttachmentId(), fineImage->GetRHIImage());

                auto coarseImage = fp->GetProbeSHAttachment(coarseLevel);
                if (coarseImage)
                    frameGraph.GetAttachmentDatabase().ImportImage(coarseImage->GetAttachmentId(), coarseImage->GetRHIImage());
            }
        }
        else // Injection pass
        {
            uint32_t level = GetCascadeLevelFromName(passName);
            auto probeImage = fp->GetProbeSHAttachment(level);
            if (probeImage)
                frameGraph.GetAttachmentDatabase().ImportImage(probeImage->GetAttachmentId(), probeImage->GetRHIImage());

            auto historyImage = fp->GetProbeHistoryAttachment(level);
            if (historyImage)
                frameGraph.GetAttachmentDatabase().ImportImage(historyImage->GetAttachmentId(), historyImage->GetRHIImage());
        }

        ComputePass::SetupFrameGraphDependencies(frameGraph);
    }

    void CascadeInjectPass::BuildInternal()
    {
        ComputePass::BuildInternal();

        auto* scene = GetScene();
        if (!scene) return;
        auto* fp = scene->GetFeatureProcessor<CascadeFeatureProcessor>();
        if (!fp) return;

        const AZ::Name& passName = GetName();

        if (IsResolveDepthPass(passName))
        {
            // Only bind the output; the input is bound via the template's "Connections" block.
            auto depthImage = fp->GetResolvedDepthImage();
            if (depthImage)
                AttachImageToSlot(AZ::Name("ResolvedDepthOutput"), depthImage);
        }
        else if (IsDiffuseGIPass(passName))
        {
            auto probeImage = fp->GetProbeSHAttachment(0);
            if (probeImage) AttachImageToSlot(AZ::Name("CascadeProbeInput"), probeImage);

            auto outputImage = fp->GetDiffuseGIOutput();
            if (outputImage) AttachImageToSlot(AZ::Name("Output"), outputImage);

            auto depthImage = fp->GetResolvedDepthImage();
            if (depthImage) AttachImageToSlot(AZ::Name("DepthInput"), depthImage);
        }
        else if (IsMergePass(passName))
        {
            uint32_t fineLevel, coarseLevel;
            if (GetMergeLevelsFromName(passName, fineLevel, coarseLevel))
            {
                auto fineImage = fp->GetProbeSHAttachment(fineLevel);
                if (fineImage) AttachImageToSlot(AZ::Name("FineProbeInput"), fineImage);

                auto coarseImage = fp->GetProbeSHAttachment(coarseLevel);
                if (coarseImage) AttachImageToSlot(AZ::Name("CoarseProbeInput"), coarseImage);
            }
        }
        else // Injection pass
        {
            uint32_t level = GetCascadeLevelFromName(passName);
            auto probeImage = fp->GetProbeSHAttachment(level);
            if (probeImage) AttachImageToSlot(AZ::Name("ProbeSHOutput"), probeImage);

            auto historyImage = fp->GetProbeHistoryAttachment(level);
            if (historyImage) AttachImageToSlot(AZ::Name("ProbeSHHistoryInput"), historyImage);
        }
    }

    void CascadeInjectPass::FrameBeginInternal(FramePrepareParams params)
    {
        ComputePass::FrameBeginInternal(params);
    }
}