#include "Surface.h"

#include "GraphicsPipeline/RenderPass.h"
#include "GraphicsPipeline/Shader.h"
#include "Setup/Instance.h"
#include "SwapChain.h"
#include "Window.h"
#include "common_defines.h"

Surface::Surface(Window &window) : window(window)
{
    VK_ASSERT(glfwCreateWindowSurface(Instance::Get(), window, nullptr, &surface),
              "Failed to create window suface!")
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(Instance::Get(), surface, nullptr);
}

void Surface::CleanupSwapChain(bool destroySwapChain)
{
    for (const auto &renderPass : renderPasses) {
        renderPass->DestroySwapChainFramebuffers(swapChain.get());
    }

    if (destroySwapChain) {
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
    auto previousRenderPasses = renderPasses;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(*device);

    CleanupSwapChain(false);

    swapChain->Recreate();

    for (const auto renderPass : previousRenderPasses) {
        /* Create frambuffers for this swap chain */
        renderPass->CreateSwapChainFramebuffers(swapChain.get());
        renderPasses.push_back(renderPass);

        for (const auto renderPassShader : renderPass->registeredShaders) {
            renderPassShader->ChangeResolutionForRenderPass(renderPass,
                                                            swapChain->GetResolution());
        }
    }
}

void Surface::RegisterRenderPass(RenderPass& renderPass)
{
    renderPasses.push_back(&renderPass);
    renderPass.CreateSwapChainFramebuffers(swapChain.get());
}
