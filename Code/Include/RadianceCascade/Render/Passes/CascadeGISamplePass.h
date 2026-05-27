#pragma once
#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace RadianceCascade
{
    class CascadeGISamplePass : public AZ::RPI::ComputePass
    {
    public:
        AZ_RTTI(CascadeGISamplePass, "{C3D4E5F6-7890-ABCD-EF12-34567890ABCD}", AZ::RPI::ComputePass);
        AZ_CLASS_ALLOCATOR(CascadeGISamplePass, AZ::SystemAllocator);

        explicit CascadeGISamplePass(const AZ::RPI::PassDescriptor& descriptor);
        static AZ::RPI::Ptr<CascadeGISamplePass> Create(const AZ::RPI::PassDescriptor& descriptor);
    };
}