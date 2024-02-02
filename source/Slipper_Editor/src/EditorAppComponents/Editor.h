#pragma once
#include "EditorAppComponent.h"

namespace Slipper::GPU
{
    class GraphicsEngine;
    class RenderingStage;
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

    void OnViewportResize(NonOwningPtr<GPU::RenderingStage> Stage, uint32_t Width, uint32_t Height);
    NonOwningPtr<ViewportData> FetchViewportImages(NonOwningPtr<GPU::RenderingStage> Stage);


    void DrawViewport(GPU::RenderingStage &Stage);
    static void DrawUpdateInput(ImVec2 ViewportPos);
    void DrawGuizmo(GPU::RenderingStage &Stage, ImVec2 ViewportPos, ImVec2 ViewportSize);

public:
    static inline bool viewportHovered = false;

 private:
    GPU::GraphicsEngine *m_graphicsEngine = nullptr;
    std::unordered_map<NonOwningPtr<GPU::RenderingStage>, OwningPtr<ViewportData>> m_viewportsData;
};
}  // namespace Slipper::Editor