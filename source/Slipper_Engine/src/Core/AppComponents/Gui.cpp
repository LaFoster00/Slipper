#include "Gui.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include "Core/Application.h"
#include "Drawing/CommandPool.h"
#include "GraphicsEngine.h"
#include "RenderPass.h"
#include "Setup/Device.h"
#include "Setup/GraphicsSettings.h"
#include "Setup/VulkanInstance.h"
#include "Window/Window.h"
#include "common_defines.h"

namespace Slipper
{
bool Gui::m_initialized = false;
Gui::ImGuiResources *Gui::m_resources = nullptr;
Device *Gui::m_device = nullptr;
const Window *Gui::m_window = nullptr;

void Gui::ImGuiResources::CreateDescriptorPool(const VkDescriptorPoolSize *Sizes,
                                               uint32_t SizesCount,
                                               uint32_t MaxSets)
{
    VkDescriptorPoolCreateInfo imgui_pool_info{};
    imgui_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    imgui_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    imgui_pool_info.poolSizeCount = SizesCount;
    imgui_pool_info.pPoolSizes = Sizes;
    imgui_pool_info.maxSets = MaxSets;

    VK_ASSERT(vkCreateDescriptorPool(*m_device, &imgui_pool_info, nullptr, &imGuiDescriptorPool),
              "Failed to create ImGui DescriptorPool")
}

Gui::Gui(std::string_view Name) : AppComponent(Name)
{
}

void Gui::Init()
{
    if (m_initialized)
        return;

    m_device = &Device::Get();
    m_window = Application::Get().window.get();

    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    VkDescriptorPoolSize imgui_pool_size{};
    imgui_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imgui_pool_size.descriptorCount = 1;

    m_resources = new ImGuiResources();
    m_resources->CreateDescriptorPool(&imgui_pool_size, 1, Engine::MAX_FRAMES_IN_FLIGHT);

    ImGui_ImplGlfw_InitForVulkan(*m_window, true);
    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = VulkanInstance::Get();
    info.PhysicalDevice = *m_device;
    info.Device = *m_device;
    info.QueueFamily = m_device->queueFamilyIndices.graphicsFamily.value();
    info.Queue = m_device->graphicsQueue;
    info.DescriptorPool = m_resources->imGuiDescriptorPool;
    info.MinImageCount = Engine::MAX_FRAMES_IN_FLIGHT;
    info.ImageCount = Engine::MAX_FRAMES_IN_FLIGHT;
    info.MSAASamples = GraphicsSettings::Get().MSAA_SAMPLES;
    ImGui_ImplVulkan_Init(&info, m_window->GetSurface().renderPasses[0]->vkRenderPass);

    SingleUseCommandBuffer command_buffer(*GraphicsEngine::Get().memoryCommandPool);
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
    command_buffer.Submit();
    vkDeviceWaitIdle(*m_device);
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    m_initialized = true;
}

void Gui::StartNewFrame()
{
    if (!m_initialized)
        return;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    SetupDocksapce();
}

void Gui::EndNewFrame(VkCommandBuffer CommandBuffer)
{
    if (!m_initialized)
        return;

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CommandBuffer);
}

void Gui::Shutdown()
{
    if (!m_initialized)
        return;

    vkDestroyDescriptorPool(*m_device, m_resources->imGuiDescriptorPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::OnUpdate()
{
    AppComponent::OnUpdate();
}

void Gui::OnGuiRender()
{
    AppComponent::OnGuiRender();
}

void Gui::SetupDocksapce()
{
    ImGui::SetNextWindowPos({0.0f, 0.0f}, ImGuiCond_Always);

    ImGuiWindowFlags window_flags =  ImGuiWindowFlags_NoDocking;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    bool window_open = true;
    ImGui::Begin("Dockspace Window", &window_open, window_flags);
    ImGui::PopStyleVar(3);
    
    // Dockspace
    ImGui::DockSpace(
        ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}
}  // namespace Slipper