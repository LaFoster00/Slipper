#include "Gui.h"

#include <imgui_internal.h>

#include "Core/Application.h"
#include "Drawing/CommandPool.h"
#include "GraphicsEngine.h"
#include "RenderPass.h"
#include "Setup/GraphicsSettings.h"
#include "Window.h"

namespace Slipper
{
Device *Gui::m_device = nullptr;

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

Gui::Gui(std::string_view Name, NonOwningPtr<RenderPass> RenderPass, bool InstallCallbacks)
    : AppComponent(Name), m_renderPass(RenderPass), m_installCallbacks(InstallCallbacks)
{
}

void Gui::Init()
{
    if (m_initialized)
        return;

    m_device = &Device::Get();

    // For each context we can initialize a new Imgui vulkan backend and therefore a different
    // render pass
    m_context = ImGui::CreateContext(m_fontAtlas);
    ImGui::SetCurrentContext(m_context);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    VkDescriptorPoolSize imgui_pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 64},
                                               {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64},
                                               {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 64},
                                               {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 64},
                                               {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 64},
                                               {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 64},
                                               {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 64},
                                               {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 64},
                                               {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 64},
                                               {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 64},
                                               {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 64}};

    m_resources = new ImGuiResources();
    m_resources->CreateDescriptorPool(imgui_pool_sizes, std::size(imgui_pool_sizes), 1024);

    const auto &window = Application::Get().window;
    ImGui_ImplGlfw_InitForVulkan(*window, m_installCallbacks);

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

    ImGui_ImplVulkan_Init(&info, m_renderPass->vkRenderPass);

    if (!m_fontAtlas) {
        // Create font atlas
        SingleUseCommandBuffer command_buffer(*GraphicsEngine::Get().memoryCommandPool);
        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
        command_buffer.Submit();
        vkDeviceWaitIdle(*m_device);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
        m_fontAtlas = ImGui::GetIO().Fonts;
    }

    m_initialized = true;
}

void Gui::StartNewFrame() const
{
    ImGui::SetCurrentContext(m_context);
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGuizmo::BeginFrame();
    SetupDocksapce();
}

void Gui::EndNewFrame(NonOwningPtr<RenderingStage> RenderingStage) const
{
    ImGui::Render();
    const auto draw_data = ImGui::GetDrawData();

    RenderingStage->SubmitSingleDrawCommand(m_renderPass, [=](const VkCommandBuffer &CommandBuffer) {
        ImGui_ImplVulkan_RenderDrawData(draw_data, CommandBuffer);
    });
}

void Gui::Shutdown()
{
    if (!m_initialized)
        return;
    ImGui::SetCurrentContext(m_context);
    vkDestroyDescriptorPool(*m_device, m_resources->imGuiDescriptorPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::SetupDocksapce()
{
    ImGui::SetNextWindowPos({0.0f, 0.0f}, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
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
    // window_flags |= ImGuiWindowFlags_NoInputs ;
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