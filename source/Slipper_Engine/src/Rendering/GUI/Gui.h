#pragma once

#include "Engine.h"

namespace Slipper
{
class RenderPass;
class Device;
class Window;

class Gui
{
    struct ImGuiResources
    {
        VkDescriptorPool imGuiDescriptorPool;

        void CreateDescriptorPool(const VkDescriptorPoolSize *Sizes,
                                  uint32_t SizesCount,
                                  uint32_t MaxSets);
    };

 public:
    static void Init(const Window &Window, const RenderPass &RenderPass);
    static void StartNewFrame();
    static void EndNewFrame(VkCommandBuffer CommandBuffer);
    static void Shutdown();

 private:
    static void SetupDocksapce();

 private:
    static bool m_initialized;
    static Device *m_device;
    static ImGuiResources *m_resources;
    static const Window *m_window;
};
}  // namespace Slipper