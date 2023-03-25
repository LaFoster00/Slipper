#include "Surface.h"

#include "Shader/Shader.h"
#include "SurfaceSwapChain.h"
#include "Window.h"

namespace Slipper
{
Surface::Surface(const Window &Window) : window(Window)
{
    VK_ASSERT(glfwCreateWindowSurface(VulkanInstance::Get(), window, nullptr, &vkSurface),
              "Failed to create window suface!");
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