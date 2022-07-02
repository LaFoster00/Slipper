#pragma once

#include "common_includes.h"

#include <functional>
#include <unordered_set>
#include <vector>

#include "DeviceDependentObject.h"

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
    static void SetSetupDefaultAssets(bool value = true);

    static GraphicsEngine &Get()
    {
        if (!instance)
        {
            instance = new GraphicsEngine();
        }
        return *instance;
    }

    static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }

    void SetupDefaultAssets();
    void CreateSyncObjects();
    RenderPass *CreateRenderPass(const VkFormat attachmentFormat);
    Shader *SetupDebugRender(Surface &surface);
    void SetupSimpleDraw();

    void AddRepeatedDrawCommand(
        std::function<void(const VkCommandBuffer &, const RenderPass &)> command);

    void DrawFrame();

    void OnWindowResized(GLFWwindow *window, int width, int height);

 private:
    GraphicsEngine();
    ~GraphicsEngine();

 public:
	uint32_t currentFrame = 0;

    std::unordered_set<Surface *> surfaces;
    std::vector<std::unique_ptr<Shader>> shaders;
    std::vector<std::unique_ptr<RenderPass>> renderPasses;
    std::vector<std::unique_ptr<Mesh>> meshes;

    // Render commands
    CommandPool *renderCommandPool;
    std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>
        repeatedRenderCommands;

    CommandPool *memoryCommandPool;

 private:
    static GraphicsEngine *instance;
    static bool bSetupDefaultAssets;

    bool m_framebufferResized = false;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};