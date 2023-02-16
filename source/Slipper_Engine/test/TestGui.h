#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Core/AppComponent.h"

namespace Slipper
{
    class GraphicsEngine;
}

class TestGui : public Slipper::AppComponent
{
public:
    void OnGuiRender() override;
    void Init() override;

private:
    Slipper::GraphicsEngine * m_graphicsEngine = nullptr;
    std::vector<VkImageView> m_viewportImages;
    std::vector<void *> m_imguiViewportImages; 
};
