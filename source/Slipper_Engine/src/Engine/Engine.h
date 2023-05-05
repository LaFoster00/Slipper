#pragma once

namespace Slipper
{
class VulkanInstance;

namespace Engine
{
inline constexpr bool LINEAR_WINDOW_COLOR = true;
inline constexpr VkFormat TARGET_WINDOW_COLOR_FORMAT = VK_FORMAT_B8G8R8A8_UNORM;
inline constexpr VkFormat TARGET_VIEWPORT_COLOR_FORMAT = LINEAR_WINDOW_COLOR ?
                                                             VK_FORMAT_B8G8R8A8_SRGB :
                                                             VK_FORMAT_B8G8R8A8_UNORM;
inline constexpr VkFormat TARGET_VIEWPORT_TEXTURE_FORMAT = TARGET_VIEWPORT_COLOR_FORMAT ==
                                                                   VK_FORMAT_B8G8R8A8_SRGB ?
                                                               VK_FORMAT_R8G8B8A8_SRGB :
                                                               VK_FORMAT_R8G8B8A8_UNORM;
inline constexpr VkColorSpaceKHR TARGET_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
inline constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
inline uint64_t FRAME_COUNT = 0;

inline bool EnableValidationLayers = true;

inline const std::vector<const char *> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

inline const std::vector<const char *> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

inline const std::vector<vk::ValidationFeatureEnableEXT> PRINTF_ENABLES = {
#ifdef SHADER_PRINTF_ENABLED
    vk::ValidationFeatureEnableEXT::eDebugPrintf,
#endif
};

}  // namespace Engine
}  // namespace Slipper