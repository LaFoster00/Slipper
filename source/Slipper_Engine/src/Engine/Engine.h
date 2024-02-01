#pragma once

namespace Slipper::Engine
{
enum class GpuBackend
{
    Vulkan = 0
};

constexpr static GpuBackend GPU_BACKEND = GpuBackend::Vulkan;
}  // namespace Slipper::Engine