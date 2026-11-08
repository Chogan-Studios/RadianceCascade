#pragma once
#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace RadianceCascade
{
    class CascadeRayMarchPass final : public AZ::RPI::ComputePass
    {
    public:
        AZ_RTTI(CascadeRayMarchPass, "{DEADBEEF-CAFE-FACE-1234-567890ABCDEF}", AZ::RPI::ComputePass);
        AZ_CLASS_ALLOCATOR(CascadeRayMarchPass, AZ::SystemAllocator);
        static AZ::RPI::Ptr<CascadeRayMarchPass> Create(const AZ::RPI::PassDescriptor& descriptor);

    protected:
        CascadeRayMarchPass(const AZ::RPI::PassDescriptor& descriptor);
        ~CascadeRayMarchPass() = default;

        void BuildInternal() override;
    };
}