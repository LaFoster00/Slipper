#pragma once
#include "Core/AppComponent.h"

namespace Slipper
{
class GraphicsEngine;
}

class TestGui : public Slipper::AppComponent
{
 public:
    TestGui() : AppComponent("TestGui")
    {
    }
    void OnGuiRender() override;
    void Init() override;

    void OnViewportResize(uint32_t Width, uint32_t Height);

 private:
    Slipper::GraphicsEngine *m_graphicsEngine = nullptr;
    std::vector<VkImageView> m_viewportImages;
    std::vector<VkDescriptorSet> m_imguiViewportImages;
}; 
