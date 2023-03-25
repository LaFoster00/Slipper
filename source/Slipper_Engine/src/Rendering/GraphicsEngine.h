#pragma once

#include "Drawing/CommandPool.h"

namespace Slipper
{
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

    void SetupDebugResources();
    void SetupDebugRender(Surface &Surface);
    void SetupSimpleDraw();
    void CreateSyncObjects();
    RenderPass *CreateRenderPass(const std::string &Name,
                                 VkFormat RenderingFormat,
                                 VkFormat DepthFormat,
                                 bool ForPresentation);
    void DestroyRenderPass(RenderPass *RenderPass);
    void CreateViewportSwapChain() const;
    void RecreateViewportSwapChain(uint32_t Width, uint32_t Height) const;
    Entity &GetDefaultCamera();

    void AddWindow(Window &Window);

    void SubmitDraw(const RenderPass *RenderPass,
                    const Shader *Shader,
                    const Mesh *Mesh,
                    const glm::mat4 &Transform);
    void SubmitSingleDrawCommand(const RenderPass *RenderPass,
                                 std::function<void(const VkCommandBuffer &)> Command);
    void SubmitRepeatedDrawCommand(
        std::function<void(const VkCommandBuffer &, const RenderPass &)> Command);

    void BeginUpdate();
    void EndUpdate();
    void BeginGuiUpdate();
    void EndGuiUpdate();
    void Render();

    static void OnViewportResize(uint32_t Width, uint32_t Height);
    static void OnWindowResized(Window *Window, int Width, int Height);

    [[nodiscard]] VkCommandBuffer GetCurrentGuiCommandBuffer() const
    {
        return guiCommandPool->vkCommandBuffers[currentFrame];
    }

    [[nodiscard]] uint32_t GetCurrentImageIndex() const
    {
        return m_currentImageIndex;
    }

 private:
    GraphicsEngine();
    ~GraphicsEngine();

 public:
    uint32_t currentFrame = 0;

    std::unordered_set<NonOwningPtr<Window>> windows;

    NonOwningPtr<RenderPass> viewportRenderPass = nullptr;
    std::unique_ptr<OffscreenSwapChain> viewportSwapChain;
    //TODO put these textures into the offscreenSwapChain and enable them conditionally
    std::vector<std::unique_ptr<Texture2D>> viewportPresentationTextures;

    NonOwningPtr<RenderPass> windowRenderPass = nullptr;
    std::unordered_map<std::string, std::unique_ptr<RenderPass>> renderPasses;
    std::unordered_map<NonOwningPtr<RenderPass>, std::string> renderPassNames;

    // Draw Commands
    std::unique_ptr<CommandPool> drawCommandPool;
    std::unordered_map<NonOwningPtr<const RenderPass>,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleDrawCommands;
    std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>
        repeatedDrawCommands;

    // Gui Commands
    std::unique_ptr<CommandPool> guiCommandPool;
    std::unordered_map<NonOwningPtr<const RenderPass>,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleGuiCommands;
    std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>
        repeatedGuiCommands;

    // Memory Transfer Commands
    std::unique_ptr<CommandPool> memoryCommandPool;

 private:
    NonOwningPtr<Surface> surface = nullptr;

    static GraphicsEngine *m_graphicsInstance;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    uint32_t m_currentImageIndex = 0;
    NonOwningPtr<Surface> m_currentSurface = nullptr;
    NonOwningPtr<RenderPass> m_currentRenderPass = nullptr;

    Entity m_defaultCamera;
};
}  // namespace Slipper