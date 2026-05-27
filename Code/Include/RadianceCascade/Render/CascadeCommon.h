#pragma once

#include <cstdint>
#include <AzCore/std/containers/array.h>

namespace RadianceCascade
{
    constexpr uint32_t MaxCascadeLevels = 5;
    constexpr uint32_t SpecularCascadeIndex = 0; // finest cascade holds octahedral map

    // Maximum number of probes per cascade level (fixed for now)
    constexpr uint32_t MaxProbesPerLevel[MaxCascadeLevels] = { 4096, 2048, 1024, 512, 256 };

    // Spherical harmonics coefficients (L0-L2)
    constexpr uint32_t SHCoeffCount = 9;
    // Octahedral map resolution per probe (specular cascade)
    constexpr uint32_t OctahedralMapSize = 8;

    enum class InjectionMode : uint32_t
    {
        Software = 0,
        HardwareRT = 1
    };
}