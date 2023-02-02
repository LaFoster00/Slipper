#pragma once

#define GLFW_INCLUDE_VULKAN
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>

#include <vector>

namespace Engine
{
static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
static uint64_t FRAME_COUNT = 0;

static bool EnableValidationLayers = true;

static const std::vector<const char *> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

static const std::vector<const char *> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

static const std::vector<VkValidationFeatureEnableEXT> PRINTF_ENABLES = {
#ifdef SHADER_PRINTF_ENABLED
    VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
#endif
};

}  // namespace Engine