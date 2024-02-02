#include "../vk_Surface.h"

#include "Window.h"

namespace Slipper
{
Surface::Surface(const Window &Window) : window(Window)
{
    VkSurfaceKHR surface_pointer = vkSurface;
    VK_ASSERT(glfwCreateWindowSurface(VulkanInstance::Get(), window, nullptr, &surface_pointer),
              "Failed to create window suface!");
    vkSurface = surface_pointer;
}

Surface::~Surface()
{
    CleanupSwapChain(true);
    vkDestroySurfaceKHR(VulkanInstance::Get(), vkSurface, nullptr);
}

void Surface::CleanupSwapChain(const bool DestroySwapChain)
{
    if (DestroySwapChain) {
        swapChain.reset();
    }
}

void Surface::CreateSwapChain()
{
    if (!swapChain)
        swapChain = new SurfaceSwapChain(*this);
}
}  // namespace Slipper