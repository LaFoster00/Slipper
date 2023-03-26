#pragma once

#include "Core/AppComponent.h"

namespace Slipper
{
	class RenderingStage;
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
	void EndNewFrame(NonOwningPtr<RenderingStage> RenderingStage) const;
    NonOwningPtr<ImGuiContext> GetImguiContext() const
    {
        return m_context;
    }

 private:
	void SetupDocksapce() const;

 public:
    void Init() override;
    void Shutdown() override;

 private:
    static Device *m_device;

    static inline ImFontAtlas *m_fontAtlas = nullptr;

	bool m_initialized = false;
    bool m_installCallbacks;
    
	ImGuiResources *m_resources = nullptr;
    ImGuiContext *m_context = nullptr;
    NonOwningPtr<RenderPass> m_renderPass;
};
}  // namespace Slipper