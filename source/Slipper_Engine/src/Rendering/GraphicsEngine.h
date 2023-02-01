#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "DeviceDependentObject.h"
#include "Drawing/Sampler.h"

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

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
    static void SetSetupDefaultAssets(bool Value = true);

    static GraphicsEngine &Get()
    {
        if (!instance) {
            instance = new GraphicsEngine();
        }
        return *instance;
    }

    static void Destroy()
    {
        Sampler::DestroyDefaultSamplers();
        delete instance;
        instance = nullptr;
    }

    void SetupDefaultAssets();
    void CreateSyncObjects();
    RenderPass *CreateRenderPass(const VkFormat AttachmentFormat, const VkFormat DepthFormat);
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

    std::unordered_set<Surface *> surfaces;
    std::vector<std::unique_ptr<Shader>> shaders;
    std::vector<std::unique_ptr<Texture>> textures;
    std::unique_ptr<Texture2D> depthBuffer;
    std::vector<std::unique_ptr<RenderPass>> renderPasses;
    std::vector<std::unique_ptr<Model>> models;

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