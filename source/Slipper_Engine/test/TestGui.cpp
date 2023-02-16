#include "TestGui.h"

#include <imgui.h>

void TestGui::OnGuiRender()
{
    AppComponent::OnGuiRender();

    bool open = true;


    //ImGui::Begin("Viewport", &open);
    //const auto viewport_texture = ImGui_ImplVulkan_AddTexture(Slipper::Sampler::GetLinearSampler(), Slipper::Application::Get().GetViewportTextureView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //auto viewport_size = Slipper::Application::Get().window->GetSize();
    //ImGui::Image(viewport_texture, {viewport_size.x, viewport_size.y}); 
    //ImGui::End();

    
    ImGui::ShowDemoWindow(&open);
}
