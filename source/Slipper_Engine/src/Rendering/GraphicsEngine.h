#pragma once

#include "Drawing/CommandPool.h"
#include "RenderingStage.h"

namespace Slipper
{
class RenderingStage;
class OffscreenSwapChain;
class Model;
class Texture2D;
class Texture;
class CommandPool;
class Shader;
class RenderPass;
class Mesh;
class VertexBuffer;
class Device;
class Window;
class Surface;

class GraphicsEngine : DeviceDependentObject
{
 public:
    static GraphicsEngine &Get()
    {
        return *m_graphicsInstance;
    }

    static void Init();

    static void Shutdown();

    static void SetupDebugResources();
    void SetupDebugRender(Surface &Surface) const;
    void SetupSimpleDraw() const;

    RenderPass *CreateRenderPass(const std::string &Name,
                                 vk::Format RenderingFormat,
                                 vk::Format DepthFormat,
                                 bool ForPresentation);
    void DestroyRenderPass(RenderPass *RenderPass);

    void AddWindow(Window &Window);
    NonOwningPtr<RenderingStage> AddRenderingStage(
        std::string Name,
        NonOwningPtr<SwapChain> SwapChain,
        VkQueue CommandQueue,
        uint32_t CommandQueueFamilyIndex,
        bool NativeSwapChain,
        int32_t CommandBufferCount = Engine::MAX_FRAMES_IN_FLIGHT);

    void NewFrame() const;
    void BeginRenderingStage(std::string_view Name);
    void EndRenderingStage();
    void EndFrame();

    static void OnViewportResize(NonOwningPtr<RenderingStage> Stage,
                                 uint32_t Width,
                                 uint32_t Height);
    static void OnWindowResized(Window *Window, int Width, int Height);

    static Entity GetDefaultCamera();

    [[nodiscard]] VkCommandBuffer GetCurrentGuiCommandBuffer() const
    {
        return viewportRenderingStage->commandPool->vkCommandBuffers[m_currentFrame];
    }

    [[nodiscard]] uint32_t GetCurrentFrame() const
    {
        return m_currentFrame;
    }

    [[nodiscard]] NonOwningPtr<CommandPool> GetViewportCommandPool() const
    {
        return viewportRenderingStage->commandPool;
    }

 private:
    GraphicsEngine();
    ~GraphicsEngine();

 public:
    std::vector<NonOwningPtr<Window>> windows;

    NonOwningPtr<RenderPass> viewportRenderPass = nullptr;
    OwningPtr<OffscreenSwapChain> viewportSwapChain;
    NonOwningPtr<RenderingStage> viewportRenderingStage;

    NonOwningPtr<RenderPass> windowRenderPass = nullptr;
    NonOwningPtr<RenderingStage> windowRenderingStage;

    std::unordered_map<std::string, std::unique_ptr<RenderPass>> renderPasses;
    std::unordered_map<NonOwningPtr<RenderPass>, std::string> renderPassNames;
    std::unordered_map<std::string, OwningPtr<RenderingStage>> renderingStages;

    // Memory Transfer Commands
    std::unique_ptr<CommandPool> memoryCommandPool;

 private:
    NonOwningPtr<Surface> surface = nullptr;

    static GraphicsEngine *m_graphicsInstance;

    std::vector<VkFence> m_inFlightFences;

    uint32_t m_currentFrame = 0;
    NonOwningPtr<RenderPass> m_currentRenderPass = nullptr;
    NonOwningPtr<RenderingStage> m_currentRenderingStage = nullptr;
};
}  // namespace Slipper