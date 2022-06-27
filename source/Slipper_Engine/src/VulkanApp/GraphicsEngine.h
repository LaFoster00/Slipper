#pragma once

#include "common_includes.h"

#include "GraphicsPipeline/Shader.h"
#include "Presentation/SwapChain.h"
#include "Drawing/Framebuffer.h"
#include "GraphicsPipeline/GraphicsPipeline.h"
#include "GraphicsPipeline/RenderPass.h"
#include "Drawing/CommandPool.h"

#include <vector>
#include <functional>

class Device;
class Window;
class Surface;

class GraphicsEngine
{
public:
    GraphicsEngine() = delete;
    GraphicsEngine(Device &device, bool setupDefaultAssets = true);
    ~GraphicsEngine();

    void SetupDefaultAssets();

    void CreateSwapChain(Window &window, Surface &surface);
    void CreateSyncObjects();

    GraphicsPipeline &SetupSimpleRenderPipeline(Window &window, Surface &surface);
    void SetupSimpleDraw();

    void AddRepeatedDrawCommand(std::function<void(VkCommandBuffer &)> command);

    void DrawFrame();

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

    std::vector<SwapChain> swapChains;
    std::vector<RenderPass> renderPasses;

    std::vector<GraphicsPipeline> graphicsPipelines;

    std::vector<CommandPool> commandPools;

    std::vector<std::function<void(VkCommandBuffer &)>> repeatedRenderCommands;

private:
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};