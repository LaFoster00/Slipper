#include "Editor.h"

#include <imgui_internal.h>

#include "Core/AppComponents/Gui.h"
#include "Core/Application.h"
#include "Drawing/Sampler.h"
#include "EntityOutliner.h"
#include "GraphicsEngine.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Texture/Texture2D.h"

namespace Slipper::Editor
{
void Editor::Init()
{
    AppComponent::Init();

    Application::Get().AddViewportResizeCallback(
        std::bind(&Editor::OnViewportResize, this, std::placeholders::_1, std::placeholders::_2));
    m_graphicsEngine = &GraphicsEngine::Get();
}

void Editor::Shutdown()
{
    AppComponent::Shutdown();
}

void Editor::OnUpdate()
{
    AppComponent::OnUpdate();
}

void Editor::OnEditorGuiUpdate()
{
    bool open = true;

    const auto swap_chain =
        m_graphicsEngine->viewportRenderingStage->GetSwapChain<OffscreenSwapChain>();
    const auto &image_views = swap_chain->GetVkImageViews();
    if (m_viewportImages.empty() && !image_views.empty()) {

        m_viewportImages.resize(swap_chain->numImages);
        for (uint32_t i{0}; i < swap_chain->numImages; i++) {
            m_viewportImages[i] = swap_chain->presentationTextures[i]->imageInfo.view;
        }

        for (const auto viewport_image : m_viewportImages) {
            m_imguiViewportImages.push_back(
                ImGui_ImplVulkan_AddTexture(Sampler::GetLinearSampler(),
                                            viewport_image,
                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        }
    }

    const auto current_frame = m_graphicsEngine->viewportRenderingStage->GetCurrentImageIndex();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport", &open);
    static ImVec2 last_window_size = {0, 0};
    auto window_size = ImGui::GetContentRegionAvail();
    window_size.x = std::max(1.0f, window_size.x);
    window_size.y = std::max(1.0f, window_size.y);
    if (last_window_size.x != window_size.x || last_window_size.y != window_size.y) {
        Application::Get().OnViewportResize(static_cast<uint32_t>(window_size.x),
                                            static_cast<uint32_t>(window_size.y));
    }
    ImGui::Image(m_imguiViewportImages[current_frame], window_size);
    //ImGui::SetWindowHitTestHole(ImGui::GetCurrentWindow(), ImGui::GetWindowPos(), window_size);
    if (ImGui::IsItemHovered()) {
        viewportHovered = true;
        ImGui::CaptureMouseFromApp(false);
    }
    last_window_size = window_size;
    ImGui::End();
    ImGui::PopStyleVar();

    // ImGui::ShowDemoWindow(&open);

    EntityOutliner::DrawEntity(m_graphicsEngine->GetDefaultCamera());
}

void Editor::OnViewportResize(uint32_t Width, uint32_t Height)
{
    for (const auto imgui_viewport_image : m_imguiViewportImages) {
        ImGui_ImplVulkan_RemoveTexture(imgui_viewport_image);
    }
    m_imguiViewportImages.clear();

    m_viewportImages.clear();

    const auto swap_chain =
        m_graphicsEngine->viewportRenderingStage->GetSwapChain<OffscreenSwapChain>();
    m_viewportImages.resize(swap_chain->numImages);
    for (uint32_t i{0}; i < swap_chain->numImages; i++) {
        m_viewportImages[i] = swap_chain->presentationTextures[i]->imageInfo.view;
    }

    m_imguiViewportImages.reserve(swap_chain->numImages);
    for (const auto viewport_image : m_viewportImages) {
        m_imguiViewportImages.push_back(
            ImGui_ImplVulkan_AddTexture(Sampler::GetLinearSampler(),
                                        viewport_image,
                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }
}
}  // namespace Slipper::Editor