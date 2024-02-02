#pragma once

namespace Slipper::GPU
{
    enum class SampleCount
    {
        e1 = 1 << 0,
        e2 = 1 << 1,
        e4 = 1 << 2,
        e8 = 1 << 3,
        e16 = 1 << 4,
        e32 = 1 << 5,
        e64 = 1 << 6
    };

    struct GraphicsSettings
    {
        inline static SampleCount MSAA_SAMPLES = SampleCount::e1;
    };
}  // namespace Slipper::GPU
