#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "DeviceDependentObject.h"
#include "GUI/Gui.h"

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
    void AddRepeatedDrawCommand(
        std::function<void(const VkCommandBuffer &, const RenderPass &)> Command);
    void DrawFrame();
    void OnWindowResized();

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
    std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>
        repeatedRenderCommands;

    CommandPool *memoryCommandPool;

 private:
    Surface *surface = nullptr;

    static GraphicsEngine *m_graphicsInstance;

    bool m_framebufferResized = false;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};
}