#pragma once

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
    explicit Gui(std::string_view Name, NonOwningPtr<RenderPass> RenderPass, bool InstallCallbacks = false);
	void StartNewFrame() const;
    static void EndNewFrame(VkCommandBuffer CommandBuffer);

 private:
    static void SetupDocksapce();

 public:
    void Init() override;
    void Shutdown() override;

 private:
	bool m_initialized = false;
    bool m_installCallbacks;
    static Device *m_device;
	ImGuiResources *m_resources = nullptr;
    ImGuiContext *m_context = nullptr;
    NonOwningPtr<RenderPass> m_renderPass;
};
}  // namespace Slipper