#pragma once
#include "Vulkan/vk_DeviceDependentObject.h"
#include "Vulkan/vk_RenderingStage.h"

namespace Slipper
{
    class Window;
}

namespace Slipper::GPU
{
    class Context;
    class CommandPool;

    namespace Vulkan
    {
        class VKRenderingStage;
        class OffscreenSwapChain;
        class SwapChain;
        class RenderPass;
        class Surface;
    }

    class GraphicsEngine : Vulkan::DeviceDependentObject
    {
     public:
        static GraphicsEngine &Get()
        {
            return *m_graphicsInstance;
        }

        static void Init();

        static void Shutdown();

        static void SetupDebugResources();
        void SetupDebugRender(Context &Context) const;
        void SetupSimpleDraw() const;

        Vulkan::RenderPass *CreateRenderPass(const std::string &Name,
                                             vk::Format RenderingFormat,
                                             vk::Format DepthFormat,
                                             bool ForPresentation);
        void DestroyRenderPass(Vulkan::RenderPass *RenderPass);

        void AddWindow(Window &Window);
        NonOwningPtr<RenderingStage> AddRenderingStage(std::string Name,
                                                                 NonOwningPtr<Vulkan::SwapChain> SwapChain,
                                                                 bool NativeSwapChain);

        void NewFrame() const;
        void BeginRenderingStage(std::string_view Name);
        void EndRenderingStage();
        void EndFrame();

        static void OnViewportResize(NonOwningPtr<RenderingStage> Stage, uint32_t Width, uint32_t Height);
        static void OnWindowResized(Window *Window, int Width, int Height);

        static Entity GetDefaultCamera();

        [[nodiscard]] VkCommandBuffer GetCurrentGuiCommandBuffer() const
        {
            return viewportRenderingStage->GetGraphicsCommandPool()->GetCurrentCommandBuffer();
        }

        [[nodiscard]] uint32_t GetCurrentFrame() const
        {
            return m_currentFrame;
        }

        [[nodiscard]] NonOwningPtr<CommandPool> GetViewportCommandPool() const
        {
            return viewportRenderingStage->GetGraphicsCommandPool();
        }

     private:
        GraphicsEngine();
        ~GraphicsEngine();

     public:
        std::vector<NonOwningPtr<Window>> windows;

        NonOwningPtr<Vulkan::RenderPass> viewportRenderPass = nullptr;
        OwningPtr<Vulkan::OffscreenSwapChain> viewportSwapChain;
        NonOwningPtr<RenderingStage> viewportRenderingStage;

        NonOwningPtr<Vulkan::RenderPass> windowRenderPass = nullptr;
        NonOwningPtr<RenderingStage> windowRenderingStage;

        std::unordered_map<std::string, std::unique_ptr<Vulkan::RenderPass>> renderPasses;
        std::unordered_map<NonOwningPtr<Vulkan::RenderPass>, std::string> renderPassNames;
        std::unordered_map<std::string, OwningPtr<RenderingStage>> renderingStages;

        // Memory Transfer Commands
        std::unique_ptr<CommandPool> memoryCommandPool;

     private:
        NonOwningPtr<Vulkan::Surface> surface = nullptr;

        static GraphicsEngine *m_graphicsInstance;

        std::vector<vk::Fence> m_computeInFlightFences;
        std::vector<vk::Fence> m_renderingInFlightFences;

        uint32_t m_currentFrame = 0;
        NonOwningPtr<Vulkan::RenderPass> m_currentRenderPass = nullptr;
        NonOwningPtr<RenderingStage> m_currentRenderingStage = nullptr;
    };
}  // namespace Slipper::GPU
