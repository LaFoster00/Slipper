#include "Surface.h"

#include "RenderPass.h"
#include "Setup/VulkanInstance.h"
#include "Shader/Shader.h"
#include "SurfaceSwapChain.h"
#include "Window.h"
#include "common_defines.h"

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
    if (&GraphicsEngine::Get() != nullptr) {
        for (const auto &render_pass : renderPasses) {
            if (GraphicsEngine::Get().renderPassNames.contains(render_pass)) {
                render_pass->DestroySwapChainFramebuffers(swapChain.get());
            }
            else {
                renderPasses.erase(std::ranges::find(renderPasses, render_pass));
            }
        }
    }
    renderPasses.clear();

    if (DestroySwapChain) {
        swapChain.reset();
    }
}

void Surface::CreateSwapChain()
{
    if (!swapChain)
        swapChain = std::make_unique<SurfaceSwapChain>(*this);
}

void Surface::RecreateSwapChain(uint32_t Width, uint32_t Height)
{
    vkDeviceWaitIdle(Device::Get());

    // CleanupSwapChain(false);
    swapChain->Recreate(Width, Height);

    for (const auto render_pass : renderPasses) {
        /* Create frambuffers for this swap chain */
        render_pass->RecreateSwapChainResources(swapChain.get());
    }
}

void Surface::RegisterRenderPass(RenderPass &RenderPass)
{
    renderPasses.push_back(&RenderPass);
    RenderPass.CreateSwapChainFramebuffers(swapChain.get());
}

void Surface::UnregisterRenderPass(RenderPass &RenderPass)
{
    auto render_pass_it(std::ranges::find(renderPasses, &RenderPass));
    if (render_pass_it != renderPasses.end()) {
        renderPasses.erase(render_pass_it);
        RenderPass.DestroySwapChainFramebuffers(swapChain.get());
    }
}
}  // namespace Slipper