#pragma once

#include "common_includes.h"

#include "GraphicsPipeline/Shader.h"
#include "Presentation/SwapChain.h"
#include "Drawing/Framebuffer.h"
#include "GraphicsPipeline/GraphicsPipeline.h"
#include "GraphicsPipeline/RenderPass.h"
#include "Drawing/CommnadPool.h"

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
    GraphicsPipeline &SetupSimpleRenderPipeline(Window &window, Surface &surface);
    VkCommandBuffer SetupSimpleDraw(GraphicsPipeline &graphicsPipeline, uint32_t imageIndex);

private:
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat();
    VkPresentModeKHR ChooseSwapPresentMode();
    VkExtent2D ChooseSwapExtent(Window &window);

private:
    static GraphicsEngine *instance;

public:
    Device &device;

    std::vector<Shader> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> vkShaderStages;

    std::vector<SwapChain> swapChains;
    std::vector<RenderPass> renderPasses;

    std::vector<GraphicsPipeline> graphicsPipelines;

    std::vector<CommandPool> commandPools;
};