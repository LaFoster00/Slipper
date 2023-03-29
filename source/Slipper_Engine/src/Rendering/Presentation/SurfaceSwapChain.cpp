#include "SurfaceSwapChain.h"

#include "GraphicsEngine.h"
#include "Surface.h"
#include "Window.h"

namespace Slipper
{
SurfaceSwapChain::SurfaceSwapChain(Surface &Surface)
    : SwapChain({}, Engine::TARGET_WINDOW_COLOR_FORMAT),
      surface(Surface),
      vkSwapChain(VK_NULL_HANDLE)
{
    swapChainSupport = device.QuerySwapChainSupport(&Surface);
    resolution = ChoseExtent(Surface);
    auto [format, color_space] = ChooseSurfaceFormat();
    imageRenderingFormat = format;
    imageColorSpace = color_space;

    Create();
}

SurfaceSwapChain::~SurfaceSwapChain()
{
    SurfaceSwapChain::Impl_Cleanup();

    for (const auto image_available_semaphore : m_imageAvailableSemaphores) {
        vkDestroySemaphore(device, image_available_semaphore, nullptr);
    }

    for (const auto render_finished_semaphore : m_renderFinishedSemaphores) {
        vkDestroySemaphore(device, render_finished_semaphore, nullptr);
    }
}

VkPresentModeKHR SurfaceSwapChain::ChoosePresentMode() const
{
    for (const auto &available_present_mode : swapChainSupport.presentModes) {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR SurfaceSwapChain::ChooseSurfaceFormat() const
{
    for (const auto &available_format : swapChainSupport.formats) {
        if (available_format.format == Engine::TARGET_WINDOW_COLOR_FORMAT &&
            available_format.colorSpace == Engine::TARGET_COLOR_SPACE) {
            swapChainFormat = Engine::TARGET_WINDOW_COLOR_FORMAT;
            return available_format;
        }
    }

    swapChainFormat = swapChainSupport.formats[0].format;
    return swapChainSupport.formats[0];
}

void SurfaceSwapChain::Impl_Cleanup()
{
    vkDestroySwapchainKHR(device, vkSwapChain, nullptr);
    vkSwapChain = VK_NULL_HANDLE;
}

VkSwapchainKHR SurfaceSwapChain::Impl_GetSwapChain() const
{
    return vkSwapChain;
}

VkResult SurfaceSwapChain::AcquireNextImageKhr()
{
    return vkAcquireNextImageKHR(
        device,
        vkSwapChain,
        UINT64_MAX,
        m_imageAvailableSemaphores[GraphicsEngine::Get().GetCurrentFrame()],
        VK_NULL_HANDLE,
        &m_currentImageIndex);
}

uint32_t SurfaceSwapChain::GetCurrentSwapChainImageIndex() const
{
    return m_currentImageIndex;
}

void SurfaceSwapChain::Impl_Create()
{
    const VkPresentModeKHR present_mode = ChoosePresentMode();
    uint32_t image_count = Device::CapabilitiesSwapChainImageCount(swapChainSupport);

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = imageRenderingFormat;
    create_info.imageColorSpace = imageColorSpace;
    create_info.imageExtent = resolution;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t indices[] = {device.queueFamilyIndices.graphicsFamily.value(),
                                device.queueFamilyIndices.presentFamily.value()};

    if (device.queueFamilyIndices.graphicsFamily != device.queueFamilyIndices.presentFamily) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = indices;
    }
    else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;      // Optional
        create_info.pQueueFamilyIndices = nullptr;  // Optional
    }

    create_info.preTransform = swapChainSupport.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = vkSwapChain;

    VK_ASSERT(vkCreateSwapchainKHR(device, &create_info, nullptr, &vkSwapChain),
              "Failed to create swap chain!");

    image_count = 0;
    vkGetSwapchainImagesKHR(device, vkSwapChain, &image_count, nullptr);
    GetVkImages().resize(image_count);
    vkGetSwapchainImagesKHR(device, vkSwapChain, &image_count, GetVkImages().data());
    imageRenderingFormat = create_info.imageFormat;
    resolution = create_info.imageExtent;

    if (m_imageAvailableSemaphores.empty()) {
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        m_imageAvailableSemaphores.resize(Engine::MAX_FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(Engine::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i) {
            VK_ASSERT(vkCreateSemaphore(
                          device, &semaphore_info, nullptr, &m_imageAvailableSemaphores[i]),
                      "Failed to create semaphore!")
            VK_ASSERT(vkCreateSemaphore(
                          device, &semaphore_info, nullptr, &m_renderFinishedSemaphores[i]),
                      "Failed to create semaphore!")
        }
    }
}

VkExtent2D SurfaceSwapChain::ChoseExtent(const Surface &Surface) const
{
    if (swapChainSupport.capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return swapChainSupport.capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(Surface.window, &width, &height);

        VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actual_extent.width = std::clamp(actual_extent.width,
                                         swapChainSupport.capabilities.minImageExtent.width,
                                         swapChainSupport.capabilities.maxImageExtent.width);
        actual_extent.height = std::clamp(actual_extent.height,
                                          swapChainSupport.capabilities.minImageExtent.height,
                                          swapChainSupport.capabilities.maxImageExtent.height);

        return actual_extent;
    }
}
}  // namespace Slipper
