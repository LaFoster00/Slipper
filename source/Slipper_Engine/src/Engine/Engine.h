#pragma once

#include <vector>

namespace Engine
{
static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

static bool EnableValidationLayers = true;

static const std::vector<const char *> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static const std::vector<const char *> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

}  // namespace Engine