#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "DeviceDependentObject.h"
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
    void CreateSyncObjects();
    RenderPass *CreateRenderPass(const std::string &Name,
                                 VkFormat RenderingFormat,
                                 VkFormat DepthFormat,
                                 bool ForPresentation);
    void DestroyRenderPass(RenderPass *RenderPass);

    void CreateViewportSwapChain();
    void RecreateViewportSwapChain(uint32_t Width, uint32_t Height);

    void AddWindow(Window &Window);
    void SetupDebugRender(Surface &Surface);
    void SetupSimpleDraw();

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

    std::unordered_set<Window *> windows;
    std::vector<std::unique_ptr<Shader>> shaders;
    std::vector<std::unique_ptr<Texture>> textures;
    std::unique_ptr<Texture2D> depthBuffer;

    RenderPass *viewportRenderPass = nullptr;
    std::unique_ptr<OffscreenSwapChain> viewportSwapChain;
    std::vector<std::unique_ptr<Texture2D>> viewportPresentationTextures;

    RenderPass *windowRenderPass = nullptr;
    std::unordered_map<std::string, std::unique_ptr<RenderPass>> renderPasses;
    std::unordered_map<RenderPass *, std::string> renderPassNames;
    std::vector<std::unique_ptr<Model>> models;

    // Draw Commands
    std::unique_ptr<CommandPool> drawCommandPool;
    std::unordered_map<const RenderPass *,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleDrawCommands;
    std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>
        repeatedDrawCommands;

    // Gui Commands
    std::unique_ptr<CommandPool> guiCommandPool;
    std::unordered_map<const RenderPass *,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleGuiCommands;
    std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>
        repeatedGuiCommands;

    // Memory Transfer Commands
    std::unique_ptr<CommandPool> memoryCommandPool;

 private:
    Surface *surface = nullptr;

    static GraphicsEngine *m_graphicsInstance;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

 private:
    uint32_t m_currentImageIndex = 0;
    Surface *m_currentSurface = nullptr;
    RenderPass *m_currentRenderPass = nullptr;
};
}  // namespace Slipper