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
    void SetupDebugRender(Surface &Surface);
    void SetupSimpleDraw();

    RenderPass *CreateRenderPass(const std::string &Name,
                                 VkFormat RenderingFormat,
                                 VkFormat DepthFormat,
                                 bool ForPresentation);
    void DestroyRenderPass(RenderPass *RenderPass);

    void AddWindow(Window &Window);

    void BeginUpdate();
    void EndUpdate() const;
    void BeginGuiUpdate() const;
    void EndGuiUpdate() const;
    void Render();

    static void OnViewportResize(uint32_t Width, uint32_t Height);
    static void OnWindowResized(Window *Window, int Width, int Height);

    Entity &GetDefaultCamera();

    [[nodiscard]] VkCommandBuffer GetCurrentGuiCommandBuffer() const
    {
        return windowRenderingStage->commandPool->vkCommandBuffers[m_currentFrame];
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
    std::unordered_set<NonOwningPtr<Window>> windows;

    NonOwningPtr<RenderPass> viewportRenderPass = nullptr;
    OwningPtr<RenderingStage> viewportRenderingStage;

    NonOwningPtr<RenderPass> windowRenderPass = nullptr;
    OwningPtr<RenderingStage> windowRenderingStage;

    std::unordered_map<std::string, std::unique_ptr<RenderPass>> renderPasses;
    std::unordered_map<NonOwningPtr<RenderPass>, std::string> renderPassNames;

    // Memory Transfer Commands
    std::unique_ptr<CommandPool> memoryCommandPool;

 private:
    NonOwningPtr<Surface> surface = nullptr;

    static GraphicsEngine *m_graphicsInstance;

    std::vector<VkFence> m_inFlightFences;

    uint32_t m_currentFrame = 0;
    NonOwningPtr<Surface> m_currentSurface = nullptr;
    NonOwningPtr<RenderPass> m_currentRenderPass = nullptr;

    Entity m_defaultCamera;
};
}  // namespace Slipper