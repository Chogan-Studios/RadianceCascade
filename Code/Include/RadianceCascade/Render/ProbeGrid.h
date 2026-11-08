#pragma once
#include <AzCore/Math/Matrix4x4.h>
#include <AzCore/Math/Vector3.h>
#include <RadianceCascade/Render/CascadeCommon.h>

namespace RadianceCascade
{
    class ProbeGrid
    {
    public:
        virtual ~ProbeGrid() = default;
        virtual void Update(const AZ::Matrix4x4& viewProj) = 0;
        virtual AZ::Vector3 WorldToGrid(const AZ::Vector3& worldPos, uint32_t cascade) const = 0;
        virtual uint32_t GetGridSize(uint32_t cascade) const = 0;
        virtual float GetSpacing(uint32_t cascade) const = 0;
    };

    class ScreenSpaceProbeGrid final : public ProbeGrid
    {
    public:
        void Update(const AZ::Matrix4x4& viewProj) override;
        AZ::Vector3 WorldToGrid(const AZ::Vector3& worldPos, uint32_t cascade) const override;
        uint32_t GetGridSize(uint32_t cascade) const override;
        float GetSpacing(uint32_t cascade) const override;
        
    private:
        AZ::Matrix4x4 m_viewProj;
    };
}