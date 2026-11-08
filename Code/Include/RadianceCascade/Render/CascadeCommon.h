#pragma once
#include <cstdint>
#include <AzCore/Console/Console.h>

namespace RadianceCascade
{
    // Cascade i covers the distance interval [S * 2^i, S * 2^{i+1}]
    constexpr uint32_t MaxCascadeLevels = 8;
    constexpr uint32_t AngularResolution = 8; // 8x8 octahedral map per probe
    
    // Octahedral map wrapped dimensions
    constexpr uint32_t OctaTexW = AngularResolution * 2;
    constexpr uint32_t OctaTexH = AngularResolution;

    struct CascadeLevel
    {
        uint32_t m_probeGridSize;
        float    m_spacing;
        float    m_intervalStart;
        float    m_intervalEnd;
    };

    // Global CVars
    AZ_CVAR(int32_t, r_rc_mode, 1, nullptr, AZ::ConsoleFunctorFlags::Null, "0=Off, 1=ScreenSpace, 2=WorldSpace");
    AZ_CVAR(float, r_rc_probeSpacing, 0.5f, nullptr, AZ::ConsoleFunctorFlags::Null, "Base spacing for cascade 0");
    AZ_CVAR(uint32_t, r_rc_cascadeCount, MaxCascadeLevels, nullptr, AZ::ConsoleFunctorFlags::Null, "Number of active cascades");
}