#include "TestGui.h"

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include "Core/Application.h"
#include "Drawing/Sampler.h"
#include "GraphicsEngine.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Presentation/SwapChain.h"
#include "Window.h"

void TestGui::OnGuiRender()
{
    AppComponent::OnGuiRender();

    bool open = true;

    if (m_viewportImages.empty() && !m_graphicsEngine->viewportSwapChain->vkImageViews.empty()) {

        m_viewportImages.resize(Slipper::Engine::MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i{0}; i < Slipper::Engine::MAX_FRAMES_IN_FLIGHT; i++) {
            m_viewportImages[i] = m_graphicsEngine->viewportSwapChain->vkImageViews[i];
        }

        for (const auto viewport_image : m_viewportImages) {
            m_imguiViewportImages.push_back(
                ImGui_ImplVulkan_AddTexture(Slipper::Sampler::GetLinearSampler(),
                                            viewport_image,
                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        }
    }

    const auto current_frame = m_graphicsEngine->currentFrame;

    ImGui::Begin("Viewport", &open);
    const auto viewport_size = Slipper::Application::Get().window->GetSize();
    ImGui::Image(
        m_imguiViewportImages[current_frame],
        {static_cast<float>(viewport_size.width), static_cast<float>(viewport_size.height)});
    ImGui::End();

    ImGui::ShowDemoWindow(&open);
}

void TestGui::Init()
{
    AppComponent::Init();

    m_graphicsEngine = &Slipper::GraphicsEngine::Get();
}
