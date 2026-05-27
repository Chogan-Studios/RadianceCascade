#include "CascadeInjectPass.h"
#include <Atom/RPI.Public/RPISystemInterface.h>
#include <AzCore/Console/Console.h>
#include <AzCore/Name/Name.h>

namespace RadianceCascade
{
    AZ::RPI::Ptr<CascadeInjectPass> CascadeInjectPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew CascadeInjectPass(descriptor);
    }

    void CascadeInjectPass::BuildInternal()
    {
        ComputePass::BuildInternal();
        // Input/Output slots will be set by the pass template (JSON)
    }

    void CascadeInjectPass::FrameBeginInternal(AZ::RPI::Pass::FramePrepareParams params)
    {
        int32_t mode = AZ::RPI::CvarManager::GetValue<int32_t>("r_radianceCascade_mode");
        m_modeOption.SetValue(AZ::Name(mode == 0 ? "Software" : "HWRT"));
        if (m_shader)
        {
            m_shader->SetOption(m_modeOption);
        }
        ComputePass::FrameBeginInternal(params);
    }
}