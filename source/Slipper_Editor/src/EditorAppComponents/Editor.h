#pragma once
#include "EditorAppComponent.h"

namespace Slipper
{
	class SwapChain;
	class RenderingStage;
	class GraphicsEngine;
}

namespace Slipper::Editor
{
class Editor : public EditorAppComponent
{
    struct ViewportData
    {
        std::vector<VkImageView> images;
        std::vector<VkDescriptorSet> descriptors;
    };

 public:
    Editor() : EditorAppComponent("Editor")
    {
    }
    void Init() override;
    void Shutdown() override;
    void OnUpdate() override;
    void OnEditorGuiUpdate() override;

    void OnViewportResize(NonOwningPtr<RenderingStage> Stage, uint32_t Width, uint32_t Height);
    NonOwningPtr<ViewportData> FetchViewportImages(NonOwningPtr<RenderingStage> Stage);


    void DrawViewport(RenderingStage &Stage);
    static void DrawUpdateInput(ImVec2 ViewportPos);
    void DrawGuizmo(RenderingStage &Stage, ImVec2 ViewportPos, ImVec2 ViewportSize);

public:
    static inline bool viewportHovered = false;

 private:
    GraphicsEngine *m_graphicsEngine = nullptr;
    std::unordered_map<NonOwningPtr<RenderingStage>, OwningPtr<ViewportData>> m_viewportsData;
};
}  // namespace Slipper::Editor