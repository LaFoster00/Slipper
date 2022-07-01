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

class Mesh;
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
    RenderPass *CreateRenderPass(const VkFormat attachmentFormat);
    Shader *SetupDebugRender(Window &window,
                                                             Surface &surface);
    void SetupSimpleDraw();

    void AddRepeatedDrawCommand(std::function<void(const VkCommandBuffer &, const SwapChain &)> command);

    void DrawFrame();

    void OnWindowResized(GLFWwindow *window, int width, int height);

 private:
    static GraphicsEngine *instance;

 public:
    Device &device;

    uint32_t currentFrame = 0;

    std::vector<std::unique_ptr<Shader>> shaders;

    std::vector<std::unique_ptr<SwapChain>> swapChains;
    std::unordered_map<SwapChain *, std::tuple<Window &, Surface &, std::vector<RenderPass *>>>
        swapChainDependencies;

    std::vector<std::unique_ptr<RenderPass>> renderPasses;

    std::vector<std::unique_ptr<Mesh>> meshes;

    //Render commands
    CommandPool *renderCommandPool;
    std::vector<std::function<void(const VkCommandBuffer &, const SwapChain &)>> repeatedRenderCommands;

    CommandPool *memoryCommandPool;

 private:
    bool m_framebufferResized = false;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};