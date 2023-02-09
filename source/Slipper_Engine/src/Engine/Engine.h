#pragma once

#define GLFW_INCLUDE_VULKAN
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>

#include <vector>

namespace Slipper
{
class VulkanInstance;

namespace Engine
{
inline constexpr VkFormat TARGET_COLOR_FORMAT = VK_FORMAT_B8G8R8A8_SRGB;
inline constexpr VkColorSpaceKHR TARGET_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
inline constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
inline uint64_t FRAME_COUNT = 0;

inline bool EnableValidationLayers = true;

inline const std::vector<const char *> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

inline const std::vector<const char *> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

inline const std::vector<VkValidationFeatureEnableEXT> PRINTF_ENABLES = {
#ifdef SHADER_PRINTF_ENABLED
    VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
#endif
};

}  // namespace Engine
}  // namespace Slipper