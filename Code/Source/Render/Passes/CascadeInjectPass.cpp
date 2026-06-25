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
    { }

    // Extract the cascade level from an inject pass name (e.g. "CascadeProbeSHPass_C1" → 1).
    static uint32_t GetCascadeLevelFromName(const AZ::Name& passName)
    {
        AZStd::string_view name = passName.GetStringView();
        size_t pos = name.rfind("_C");
        if (pos != AZStd::string::npos)
        {
            AZStd::string_view levelStr = name.substr(pos + 2);
            return static_cast<uint32_t>(AZStd::stoi(AZStd::string(levelStr)));
        }
        return 0;
    }

    static bool IsMergePass(const AZ::Name& passName)
    {
        return passName.GetStringView().starts_with("CascadeMergePass_");
    }

    // Extract fine and coarse level indices from merge pass name.
    // "CascadeMergePass_16_8" → fineLevel=0, coarseLevel=1
    // "CascadeMergePass_8_4"  → fineLevel=1, coarseLevel=2
    static bool GetMergeLevelsFromName(const AZ::Name& passName, uint32_t& fineLevel, uint32_t& coarseLevel)
    {
        AZStd::string_view name = passName.GetStringView();
        if (name.find("_16_8") != AZStd::string::npos)
        {
            fineLevel = 0;
            coarseLevel = 1;
            return true;
        }
        if (name.find("_8_4") != AZStd::string::npos)
        {
            fineLevel = 1;
            coarseLevel = 2;
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

        if (IsMergePass(passName))
        {
            uint32_t fineLevel, coarseLevel;
            if (GetMergeLevelsFromName(passName, fineLevel, coarseLevel))
            {
                auto fineImage   = fp->GetProbeSHAttachment(fineLevel);
                auto coarseImage = fp->GetProbeSHAttachment(coarseLevel);

                if (fineImage)
                {
                    AZ::RHI::AttachmentId id = fineImage->GetAttachmentId();
                    frameGraph.GetAttachmentDatabase().ImportImage(id, fineImage->GetRHIImage());
                    AZ::RHI::ImageScopeAttachmentDescriptor desc;
                    desc.m_attachmentId = id;
                    desc.m_loadStoreAction.m_loadAction   = AZ::RHI::AttachmentLoadAction::Load;
                    desc.m_loadStoreAction.m_storeAction  = AZ::RHI::AttachmentStoreAction::Store;
                    frameGraph.UseAttachment(desc, AZ::RHI::ScopeAttachmentAccess::ReadWrite,
                                             AZ::RHI::ScopeAttachmentUsage::Shader,
                                             AZ::RHI::ScopeAttachmentStage::ComputeShader);
                }
                if (coarseImage)
                {
                    AZ::RHI::AttachmentId id = coarseImage->GetAttachmentId();
                    frameGraph.GetAttachmentDatabase().ImportImage(id, coarseImage->GetRHIImage());
                    AZ::RHI::ImageScopeAttachmentDescriptor desc;
                    desc.m_attachmentId = id;
                    desc.m_loadStoreAction.m_loadAction   = AZ::RHI::AttachmentLoadAction::Load;
                    desc.m_loadStoreAction.m_storeAction  = AZ::RHI::AttachmentStoreAction::Store;
                    frameGraph.UseAttachment(desc, AZ::RHI::ScopeAttachmentAccess::Read,
                                             AZ::RHI::ScopeAttachmentUsage::Shader,
                                             AZ::RHI::ScopeAttachmentStage::ComputeShader);
                }
            }
        }
        else  // inject pass
        {
            uint32_t level = GetCascadeLevelFromName(passName);
            auto probeImage = fp->GetProbeSHAttachment(level);
            if (!probeImage) return;

            AZ::RHI::AttachmentId attId = probeImage->GetAttachmentId();
            frameGraph.GetAttachmentDatabase().ImportImage(attId, probeImage->GetRHIImage());

            AZ::RHI::ImageScopeAttachmentDescriptor scopeDesc;
            scopeDesc.m_attachmentId = attId;
            scopeDesc.m_loadStoreAction.m_loadAction   = AZ::RHI::AttachmentLoadAction::Load;
            scopeDesc.m_loadStoreAction.m_storeAction  = AZ::RHI::AttachmentStoreAction::Store;
            frameGraph.UseAttachment(scopeDesc, AZ::RHI::ScopeAttachmentAccess::ReadWrite,
                                     AZ::RHI::ScopeAttachmentUsage::Shader,
                                     AZ::RHI::ScopeAttachmentStage::ComputeShader);
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

        if (IsMergePass(passName))
        {
            uint32_t fineLevel, coarseLevel;
            if (GetMergeLevelsFromName(passName, fineLevel, coarseLevel))
            {
                auto fineImage   = fp->GetProbeSHAttachment(fineLevel);
                auto coarseImage = fp->GetProbeSHAttachment(coarseLevel);
                if (fineImage)   AttachImageToSlot(AZ::Name("FineProbeInput"), fineImage);
                if (coarseImage) AttachImageToSlot(AZ::Name("CoarseProbeInput"), coarseImage);
            }
        }
        else  // inject pass
        {
            uint32_t level = GetCascadeLevelFromName(passName);
            auto probeImage = fp->GetProbeSHAttachment(level);
            if (probeImage)
                AttachImageToSlot(AZ::Name("ProbeSHOutput"), probeImage);
        }
    }

    void CascadeInjectPass::FrameBeginInternal(FramePrepareParams params)
    {
        ComputePass::FrameBeginInternal(params);
    }
}