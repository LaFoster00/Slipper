#pragma once

#include "Engine.h"
#include "Core/AppComponent.h"

namespace Slipper
{
class RenderPass;
class Device;
class Window;

class Gui : public AppComponent
{
    struct ImGuiResources
    {
        VkDescriptorPool imGuiDescriptorPool;

        void CreateDescriptorPool(const VkDescriptorPoolSize *Sizes,
                                  uint32_t SizesCount,
                                  uint32_t MaxSets);
    };

 public:
    void StartNewFrame();
    void EndNewFrame(VkCommandBuffer CommandBuffer);

 private:
    static void SetupDocksapce();

public:
    void Init() override;
    void Shutdown() override;
    void OnUpdate() override;
    void OnGuiRender() override;

private:
    static bool m_initialized;
    static Device *m_device;
    static ImGuiResources *m_resources;
    static const Window *m_window;
};
}  // namespace Slipper