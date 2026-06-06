#include <RadianceCascade/Render/Passes/CascadeInjectPass.h>
#include <AzCore/Debug/Trace.h>
#include <AzCore/Math/Matrix4x4.h>
#include <Atom/RPI.Public/ViewportContextBus.h>
#include <Atom/RPI.Public/ViewportContext.h>

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

        AZ_Printf("RadianceCascade", "CascadeInjectPass BuildInternal: shader=%s srg=%s\n",
            GetShader() ? "loaded" : "MISSING",
            GetShaderResourceGroup() ? "ok" : "MISSING");

        if (!GetShaderResourceGroup())
            return;

        auto idxOrigin = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_clipmapOrigin"));
        if (idxOrigin.IsValid())
            GetShaderResourceGroup()->SetConstant(idxOrigin, AZ::Vector3(0.0f, 0.0f, 0.0f));

        auto idxCellSize = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_cellSize"));
        if (idxCellSize.IsValid())
            GetShaderResourceGroup()->SetConstant(idxCellSize, 1.0f);

        auto idxGridSize = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_gridSize"));
        if (idxGridSize.IsValid())
            GetShaderResourceGroup()->SetConstant(idxGridSize, AZ::Vector3(16.0f, 16.0f, 16.0f));

        auto idxViewProj = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_viewProjMatrix"));
        if (idxViewProj.IsValid())
        {
            AZ::RPI::ViewportContextPtr viewport = AZ::RPI::ViewportContextRequests::Get()->GetDefaultViewportContext();
            if (viewport)
            {
                AZ::Matrix4x4 viewProj = viewport->GetCameraViewMatrix() * viewport->GetCameraProjectionMatrix();
                GetShaderResourceGroup()->SetConstant(idxViewProj, viewProj);
            }
            else
            {
                GetShaderResourceGroup()->SetConstant(idxViewProj, AZ::Matrix4x4::CreateIdentity());
            }
        }

        auto idxSunDir = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_sunDirection"));
        if (idxSunDir.IsValid())
            GetShaderResourceGroup()->SetConstant(idxSunDir,
                AZ::Vector3(0.5f, -1.0f, 0.2f).GetNormalized());

        auto idxSunColor = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_sunColor"));
        if (idxSunColor.IsValid())
            GetShaderResourceGroup()->SetConstant(idxSunColor, AZ::Vector3(1.0f, 0.95f, 0.8f));

        auto idxSunInt = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_sunIntensity"));
        if (idxSunInt.IsValid())
            GetShaderResourceGroup()->SetConstant(idxSunInt, 5.0f);

        auto idxAmbient = GetShaderResourceGroup()->FindShaderInputConstantIndex(AZ::Name("m_ambientIntensity"));
        if (idxAmbient.IsValid())
            GetShaderResourceGroup()->SetConstant(idxAmbient, 0.1f);

        GetShaderResourceGroup()->Compile();
    }

    void CascadeInjectPass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        ComputePass::FrameBeginInternal(params);
    }
}