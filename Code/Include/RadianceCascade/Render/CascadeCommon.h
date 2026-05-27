#pragma once
#include <cstdint>
#include <AzCore/std/containers/array.h>

namespace RadianceCascade
{
    constexpr uint32_t MaxCascadeLevels = 5;
    constexpr uint32_t SpecularCascadeIndex = 0;

    constexpr uint32_t MaxProbesPerLevel[MaxCascadeLevels] = { 4096, 2048, 1024, 512, 256 };

    constexpr uint32_t SHCoeffCount = 9;
    constexpr uint32_t OctahedralMapSize = 8;

    enum class InjectionMode : uint32_t { Software = 0, HardwareRT = 1 };
}