#pragma once
#include "Core/AppComponent.h"

namespace Slipper
{
	class GraphicsEngine;
}

namespace Slipper::Editor
{
class Editor : public AppComponent
{
 public:
    Editor() : AppComponent("Editor")
    {
    }
    void Init() override;
    void Shutdown() override;
    void OnUpdate() override;
    void OnGuiRender() override;

    void OnViewportResize(uint32_t Width, uint32_t Height);

    void RegisterEditors() const;

 private:
    GraphicsEngine *m_graphicsEngine = nullptr;
    std::vector<VkImageView> m_viewportImages;
    std::vector<VkDescriptorSet> m_imguiViewportImages;
};
}  // namespace Slipper::Editor