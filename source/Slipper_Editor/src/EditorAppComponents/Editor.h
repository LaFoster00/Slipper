#pragma once
#include "EditorAppComponent.h"

namespace Slipper
{
	class GraphicsEngine;
}

namespace Slipper::Editor
{
class Editor : public EditorAppComponent
{
 public:
    Editor() : EditorAppComponent("Editor")
    {
    }
    void Init() override;
    void Shutdown() override;
    void OnUpdate() override;
    void OnEditorGuiUpdate() override;

    void OnViewportResize(uint32_t Width, uint32_t Height);


public:
    static inline bool viewportHovered = false;

 private:
    GraphicsEngine *m_graphicsEngine = nullptr;
    std::vector<VkImageView> m_viewportImages;
    std::vector<VkDescriptorSet> m_imguiViewportImages;
};
}  // namespace Slipper::Editor