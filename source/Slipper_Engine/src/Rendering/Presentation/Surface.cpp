#include "Surface.h"

#include "GraphicsPipeline/RenderPass.h"
#include "GraphicsPipeline/Shader.h"
#include "Setup/Instance.h"
#include "SwapChain.h"
#include "Window.h"
#include "common_defines.h"

Surface::Surface(const Window &Window) : window(Window)
{
    VK_ASSERT(glfwCreateWindowSurface(Instance::Get(), Window, nullptr, &surface),
              "Failed to create window suface!")
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(Instance::Get(), surface, nullptr);
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
    device = &Device::Get();
    swapChain = std::make_unique<SwapChain>(*this);
}

void Surface::DestroyDeviceDependencies()
{
    swapChain.reset();
}

void Surface::RecreateSwapChain()
{
    int width = 0, height = 0;
    const auto previous_render_passes = renderPasses;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(*device);

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

void Surface::RegisterRenderPass(RenderPass& RenderPass)
{
    renderPasses.push_back(&RenderPass);
    RenderPass.CreateSwapChainFramebuffers(swapChain.get());
}
