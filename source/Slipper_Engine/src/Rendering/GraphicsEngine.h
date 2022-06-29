#pragma once

#include "common_includes.h"

#include "Drawing/CommandPool.h"
#include "Drawing/Framebuffer.h"
#include "GraphicsPipeline/GraphicsPipeline.h"
#include "GraphicsPipeline/RenderPass.h"
#include "GraphicsPipeline/Shader.h"
#include "Presentation/SwapChain.h"

#include <functional>
#include <vector>

class VertexBuffer;
class Device;
class Window;
class Surface;

class GraphicsEngine
{
 public:
    GraphicsEngine() = delete;
    GraphicsEngine(Device &device, bool setupDefaultAssets = true);
    ~GraphicsEngine();

    SwapChain *CreateSwapChain(Window &window, Surface &surface);
    void CleanupSwapChain(SwapChain *SwapChain);
    void RecreateSwapChain(SwapChain *SwapChain);

    void SetupDefaultAssets();
    void CreateSyncObjects();
    RenderPass *CreateRenderPass();
    GraphicsPipeline *SetupSimpleRenderPipelineForRenderPass(Window &window,
                                                             Surface &surface,
                                                             RenderPass *RenderPass);
    void SetupSimpleDraw();

    void AddRepeatedDrawCommand(std::function<void(VkCommandBuffer &)> command);

    void DrawFrame();

    void OnWindowResized(GLFWwindow *window, int width, int height);

 private:
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat();
    VkPresentModeKHR ChooseSwapPresentMode();
    VkExtent2D ChooseSwapExtent(Window &window);

 private:
    static GraphicsEngine *instance;

 public:
    Device &device;

    uint32_t currentFrame = 0;

    std::vector<Shader> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> vkShaderStages;

    std::vector<std::unique_ptr<SwapChain>> swapChains;
    std::unordered_map<SwapChain *, std::tuple<Window &, Surface &, std::vector<RenderPass *>>>
        swapChainDependencies;

    std::vector<std::unique_ptr<RenderPass>> renderPasses;

    std::unordered_map<RenderPass *, std::unique_ptr<GraphicsPipeline>> graphicsPipelines;

    std::vector<std::unique_ptr<VertexBuffer>> vertexBuffers;

    //Render commands
    CommandPool *commandPool;
    std::vector<std::function<void(VkCommandBuffer &)>> repeatedRenderCommands;

 private:
    bool m_framebufferResized = false;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};