#include "Editor.h"

#include "CameraEditor.h"
#include "ComponentEditorRegistry.h"
#include "Core/Application.h"
#include "Drawing/Sampler.h"
#include "EntityOutliner.h"
#include "GraphicsEngine.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Texture/Texture2D.h"
#include "TransformEditor.h"

namespace Slipper::Editor
{
void Editor::Init()
{
    AppComponent::Init();

    Application::Get().AddViewportResizeCallback(
        std::bind(&Editor::OnViewportResize, this, std::placeholders::_1, std::placeholders::_2));
    m_graphicsEngine = &GraphicsEngine::Get();

    RegisterEditors();
}

void Editor::Shutdown()
{
    AppComponent::Shutdown();
}

void Editor::OnUpdate()
{
    AppComponent::OnUpdate();
}

void Editor::OnGuiRender()
{
    AppComponent::OnGuiRender();

    bool open = true;

    if (m_viewportImages.empty() && !m_graphicsEngine->viewportSwapChain->vkImageViews.empty()) {

        m_viewportImages.resize(m_graphicsEngine->viewportSwapChain->numImages);
        for (uint32_t i{0}; i < m_graphicsEngine->viewportSwapChain->numImages; i++) {
            m_viewportImages[i] =
                m_graphicsEngine->viewportPresentationTextures[i]->imageInfo.view;
        }

        for (const auto viewport_image : m_viewportImages) {
            m_imguiViewportImages.push_back(
                ImGui_ImplVulkan_AddTexture(Sampler::GetLinearSampler(),
                                            viewport_image,
                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        }
    }

    const auto current_frame = m_graphicsEngine->GetCurrentImageIndex();

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
    last_window_size = window_size;
    ImGui::End();

    ImGui::ShowDemoWindow(&open);

    EntityOutliner::DrawEntity(m_graphicsEngine->GetDefaultCamera());
}

void Editor::OnViewportResize(uint32_t Width, uint32_t Height)
{
    for (const auto imgui_viewport_image : m_imguiViewportImages) {
        ImGui_ImplVulkan_RemoveTexture(imgui_viewport_image);
    }
    m_imguiViewportImages.clear();

    m_viewportImages.clear();
    m_viewportImages.resize(m_graphicsEngine->viewportSwapChain->numImages);
    for (uint32_t i{0}; i < m_graphicsEngine->viewportSwapChain->numImages; i++) {
        m_viewportImages[i] = m_graphicsEngine->viewportPresentationTextures[i]->imageInfo.view;
    }

    m_imguiViewportImages.reserve(m_graphicsEngine->viewportSwapChain->numImages);
    for (const auto viewport_image : m_viewportImages) {
        m_imguiViewportImages.push_back(
            ImGui_ImplVulkan_AddTexture(Sampler::GetLinearSampler(),
                                        viewport_image,
                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }
}

void Editor::RegisterEditors() const
{
    EditorRegistry::AddEditor<TransformEditor>();
    EditorRegistry::AddEditor<CameraEditor>();
}
}  // namespace Slipper::Editor