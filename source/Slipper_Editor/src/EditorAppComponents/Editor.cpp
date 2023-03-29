#include "Editor.h"

#include <imgui_internal.h>

#include "Core/AppComponents/Gui.h"
#include "Core/Application.h"
#include "Core/Input.h"
#include "Drawing/Sampler.h"
#include "EditorCameraSystem.h"
#include "EntityOutliner.h"
#include "GraphicsEngine.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Texture/Texture2D.h"
#include "Window.h"

namespace Slipper::Editor
{
void Editor::Init()
{
    AppComponent::Init();

    Application::Get().AddViewportResizeCallback(std::bind(&Editor::OnViewportResize,
                                                           this,
                                                           std::placeholders::_1,
                                                           std::placeholders::_2,
                                                           std::placeholders::_3));
    m_graphicsEngine = &GraphicsEngine::Get();
    m_graphicsEngine->GetDefaultCamera().AddComponent<EditorCameraComponent>();
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
    static bool open = true;

    DrawViewport(*m_graphicsEngine->viewportRenderingStage);

    ImGui::ShowMetricsWindow(&open);
    ImGui::PopStyleVar();

    // ImGui::ShowDemoWindow(&open);

    EntityOutliner::DrawEntity(m_graphicsEngine->GetDefaultCamera());
}

void Editor::OnViewportResize(NonOwningPtr<RenderingStage> Stage, uint32_t Width, uint32_t Height)
{
    const auto &viewport_data = m_viewportsData.at(Stage);
    for (const auto imgui_viewport_image : viewport_data->descriptors) {
        ImGui_ImplVulkan_RemoveTexture(imgui_viewport_image);
    }
    viewport_data->descriptors.clear();
    viewport_data->images.clear();

    FetchViewportImages(Stage);
}

NonOwningPtr<Editor::ViewportData> Editor::FetchViewportImages(NonOwningPtr<RenderingStage> Stage)
{
    if (!m_viewportsData.contains(Stage))
        m_viewportsData.emplace(Stage, new ViewportData());

    const auto &viewport_data = m_viewportsData.at(Stage);
    if (!viewport_data->images.empty())
        return viewport_data;

    const auto swap_chain =
        m_graphicsEngine->viewportRenderingStage->GetSwapChain<OffscreenSwapChain>();
    viewport_data->images.resize(swap_chain->numImages);
    for (uint32_t i{0}; i < swap_chain->numImages; i++) {
        viewport_data->images[i] = swap_chain->presentationTextures[i]->imageInfo.view;
    }

    viewport_data->descriptors.reserve(swap_chain->numImages);
    for (const auto viewport_image : viewport_data->images) {
        viewport_data->descriptors.push_back(
            ImGui_ImplVulkan_AddTexture(Sampler::GetLinearSampler(),
                                        viewport_image,
                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }

    return viewport_data;
}

void Editor::DrawViewport(RenderingStage &Stage)
{
    static bool open = true;
    auto viewport_data = FetchViewportImages(&Stage);

    const auto current_frame = Stage.GetCurrentImageIndex();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport", &open);
    static ImVec2 last_window_size = {0, 0};
    auto window_size = ImGui::GetContentRegionAvail();
    window_size.x = std::max(1.0f, window_size.x);
    window_size.y = std::max(1.0f, window_size.y);
    if (last_window_size.x != window_size.x || last_window_size.y != window_size.y) {
        Application::Get().OnViewportResize(
            &Stage, static_cast<uint32_t>(window_size.x), static_cast<uint32_t>(window_size.y));
    }
    auto viewport_pos = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
    InputManager::SetInputOffset({viewport_pos.x, viewport_pos.y});
    ImGui::Image(viewport_data->descriptors[current_frame], window_size);
    // ImGui::SetWindowHitTestHole(ImGui::GetCurrentWindow(), ImGui::GetWindowPos(), window_size);

    static bool last_frame_viewport_hovered = false;
    if (ImGui::IsItemHovered()) {
        viewportHovered = true;
        ImGui::CaptureMouseFromApp(false);
    }
    else {
        viewportHovered = false;
    }

    Input::insideWindow = viewportHovered;
    Input::enteredWindow = !last_frame_viewport_hovered && viewportHovered;
    Input::exitedWindow = last_frame_viewport_hovered && !viewportHovered;
    last_frame_viewport_hovered = viewportHovered;

    last_window_size = window_size;
    ImGui::End();
}
}  // namespace Slipper::Editor