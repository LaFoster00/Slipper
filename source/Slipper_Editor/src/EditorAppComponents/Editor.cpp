#include "Editor.h"

#include <imgui_internal.h>

#include "CameraComponent.h"
#include "Core/AppComponents/Gui.h"
#include "Core/Application.h"
#include "Input.h"
#include "Drawing/Sampler.h"
#include "EditorCameraSystem.h"
#include "EntityOutliner.h"
#include "GraphicsEngine.h"
#include "Presentation/OffscreenSwapChain.h"
#include "SceneOutliner.h"
#include "Texture/Texture2D.h"
#include "TransformComponent.h"
#include <ImGuizmo.cpp>

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
    GraphicsEngine::GetDefaultCamera().AddComponent<EditorCameraComponent>();
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

    SceneOutliner::Draw();
    if (SceneOutliner::IsEntitySelected()) {
        EntityOutliner::DrawEntity(SceneOutliner::GetSelectedEntity());
    }
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
    const auto viewport_data = FetchViewportImages(&Stage);

    const auto current_frame = Stage.GetCurrentImageIndex();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport", &open);
    static ImVec2 last_viewport_size = {0, 0};
    auto viewport_size = ImGui::GetContentRegionAvail();
    viewport_size.x = std::max(1.0f, viewport_size.x);
    viewport_size.y = std::max(1.0f, viewport_size.y);
    if (last_viewport_size.x != viewport_size.x || last_viewport_size.y != viewport_size.y) {
        Application::Get().OnViewportResize(&Stage,
                                            static_cast<uint32_t>(viewport_size.x),
                                            static_cast<uint32_t>(viewport_size.y));
    }
    const auto viewport_pos = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
    ImGui::Image(viewport_data->descriptors[current_frame], viewport_size);
    last_viewport_size = viewport_size;

    DrawUpdateInput(viewport_pos);
    DrawGuizmo(Stage, viewport_pos, viewport_size);

    ImGui::End();
}

void Editor::DrawUpdateInput(ImVec2 ViewportPos)
{
    InputManager::SetInputOffset({ViewportPos.x, ViewportPos.y});

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
}

void Editor::DrawGuizmo(RenderingStage &Stage, ImVec2 ViewportPos, ImVec2 ViewportSize)
{
    if (SceneOutliner::IsEntitySelected()) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x,
                          ImGui::GetWindowPos().y,
                          ImGui::GetWindowWidth(),
                          ImGui::GetWindowHeight());

        const Entity camera = m_graphicsEngine->GetDefaultCamera();
        const auto &camera_params = camera.GetComponent<Camera>();

        auto view = camera_params.GetView();
        auto projection = camera_params.GetProjection(ViewportSize.x / ViewportSize.y, true);


        static bool settings_open = true;
        const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                              ImGuiWindowFlags_NoDocking |
                                              ImGuiWindowFlags_AlwaysAutoResize |
                                              ImGuiWindowFlags_NoSavedSettings |
                                              ImGuiWindowFlags_NoFocusOnAppearing |
                                              ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowPos(
            ViewportPos + ImVec2(10.0f, 10.0f), ImGuiCond_Always, {0.0f, 0.0f});
        ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
        ImGui::Begin("Transform Settings", &settings_open, window_flags);

        static ImGuizmo::OPERATION current_gizmo_operation = ImGuizmo::TRANSLATE;
        static ImGuizmo::MODE current_gizmo_mode = ImGuizmo::WORLD;
        static bool use_snap = false;

        if (!Input::captureMouseCursor && viewportHovered && !ImGui::GetIO().WantTextInput) {
            if (ImGui::IsKeyPressed(ImGuiKey_W))
                current_gizmo_operation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_E))
                current_gizmo_operation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R))
                current_gizmo_operation = ImGuizmo::SCALE;
            if (ImGui::IsKeyPressed(ImGuiKey_S))
                use_snap = !use_snap;
        }
        if (ImGui::RadioButton("Translate", current_gizmo_operation == ImGuizmo::TRANSLATE))
            current_gizmo_operation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", current_gizmo_operation == ImGuizmo::ROTATE))
            current_gizmo_operation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", current_gizmo_operation == ImGuizmo::SCALE))
            current_gizmo_operation = ImGuizmo::SCALE;

        if (current_gizmo_operation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("Local", current_gizmo_mode == ImGuizmo::LOCAL))
                current_gizmo_mode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", current_gizmo_mode == ImGuizmo::WORLD))
                current_gizmo_mode = ImGuizmo::WORLD;
        }

        ImGui::Checkbox("##UseSnap", &use_snap);
        ImGui::SameLine();

        static glm::vec3 snap_translate = {0.1f, 0.1f, 0.1f};
        static float snap_rotate = 5.0f;
        static float snap_scale = 0.1f;
        glm::vec4 snap_unit;  // Needs to be vec4 otherwise this wont work
        switch (current_gizmo_operation) {
            case ImGuizmo::TRANSLATE:
                ImGui::DragFloat3("Translate Snap", glm::value_ptr(snap_translate));
                snap_unit.x = snap_translate.x;
                snap_unit.y = snap_translate.y;
                snap_unit.z = snap_translate.z;
                break;
            case ImGuizmo::ROTATE:
                ImGui::DragFloat("Angle Snap", &snap_rotate);
                snap_unit.x = snap_rotate;
                break;
            case ImGuizmo::SCALE:
                ImGui::DragFloat("Scale Snap", &snap_scale);
                snap_unit.x = snap_rotate;
                break;
            default:;
        }

        ImGui::End();

        const auto active_entity = SceneOutliner::GetSelectedEntity();
        auto &active_entity_transform = active_entity.GetComponent<Transform>();
        if (auto transform = active_entity_transform.GetModelMatrix();
            ImGuizmo::Manipulate(glm::value_ptr(view),
                                 glm::value_ptr(projection),
                                 current_gizmo_operation,
                                 current_gizmo_mode,
                                 glm::value_ptr(transform),
                                 nullptr,
                                 use_snap ? glm::value_ptr(snap_unit) : nullptr)) {

            glm::vec3 new_location;
            glm::vec3 new_rotation;
            glm::vec3 new_scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform),
                                                  glm::value_ptr(new_location),
                                                  glm::value_ptr(new_rotation),
                                                  glm::value_ptr(new_scale));

            active_entity_transform.SetLocation(new_location);
            active_entity_transform.SetRotation(new_rotation);
            active_entity_transform.SetScale(new_scale);
        }
    }
}
}  // namespace Slipper::Editor