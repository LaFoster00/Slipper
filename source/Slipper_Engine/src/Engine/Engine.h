#pragma once

#include <vector>

namespace Engine
{
    static bool EnableValidationLayers = true;

    static const std::vector<const char *> DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    static const std::vector<const char *> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    class Engine
    {
    };
}