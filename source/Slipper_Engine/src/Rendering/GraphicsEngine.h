#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "DeviceDependentObject.h"

namespace Slipper
{
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
                                 VkFormat AttachmentFormat,
                                 VkFormat DepthFormat);
    void AddWindow(Window &Window);
    void SetupDebugRender(Surface &Surface);
    void SetupSimpleDraw();

    void SubmitDraw(const RenderPass *RenderPass, const Shader *Shader, const Mesh *Mesh, const glm::mat4 &Transform);
    void SubmitSingleDrawCommand(const RenderPass *RenderPass,
                                 std::function<void(const VkCommandBuffer &)> Command);
    void SubmitRepeatedDrawCommand(
        std::function<void(const VkCommandBuffer &, const RenderPass &)> Command);
    void BeginFrame();
    void EndFrame();
    static void OnWindowResized(Window *Window, int Width, int Height);

 private:
    GraphicsEngine();
    ~GraphicsEngine();

 public:
    uint32_t currentFrame = 0;

    std::unordered_set<Window *> windows;
    std::vector<std::unique_ptr<Shader>> shaders;
    std::vector<std::unique_ptr<Texture>> textures;
    std::unique_ptr<Texture2D> depthBuffer;
    RenderPass *mainRenderPass = nullptr;
    std::unordered_map<std::string, std::unique_ptr<RenderPass>> renderPasses;
    std::vector<std::unique_ptr<Model>> models;

    // Render commands
    CommandPool *renderCommandPool;
    std::unordered_map<const RenderPass *, std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleDrawCommand;
    std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>
        repeatedDrawCommands;

    CommandPool *memoryCommandPool;

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
    VkCommandBuffer m_currentCommandBuffer = VK_NULL_HANDLE;
};
}  // namespace Slipper