#include "Surface.h"

#include "RenderPass.h"
#include "Setup/VulkanInstance.h"
#include "Shader/Shader.h"
#include "SwapChain.h"
#include "Window.h"
#include "common_defines.h"

namespace Slipper
{
Surface::Surface(const Window &Window) : window(Window)
{
    VK_ASSERT(glfwCreateWindowSurface(VulkanInstance::Get(), window, nullptr, &surface),
              "Failed to create window suface!");
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(VulkanInstance::Get(), surface, nullptr);
}

void Surface::CleanupSwapChain(const bool DestroySwapChain)
{
    for (const auto &render_pass : renderPasses) {
        render_pass->DestroySwapChainFramebuffers(swapChain.get());
    }

    if (DestroySwapChain) {
        swapChain.reset();
    }

    renderPasses.clear();
}

void Surface::CreateSwapChain()
{
    if (!swapChain)
        swapChain = std::make_unique<SwapChain>(*this);
}

void Surface::DestroyDeviceDependencies()
{
    swapChain.reset();
}

void Surface::RecreateSwapChain(int Width, int Height)
{
    const auto previous_render_passes = renderPasses;

    // This case should not be reached make sure to wait till valid resolution is reached further up the pipeline
    if (Width == 0 || Height == 0)
    {
        return;
    }

    vkDeviceWaitIdle(Device::Get());

    CleanupSwapChain(false);

    swapChain->Recreate();

    for (const auto render_pass : previous_render_passes) {
        /* Create frambuffers for this swap chain */
        render_pass->CreateSwapChainFramebuffers(swapChain.get());
        renderPasses.push_back(render_pass);

        for (const auto render_pass_shader : render_pass->registeredShaders) {
            render_pass_shader->ChangeResolutionForRenderPass(render_pass,
                                                              swapChain->GetResolution());
        }
    }
}

void Surface::RegisterRenderPass(RenderPass &RenderPass)
{
    renderPasses.push_back(&RenderPass);
    RenderPass.CreateSwapChainFramebuffers(swapChain.get());
}
}  // namespace Slipper