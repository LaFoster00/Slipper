#include "Application.h"

#include "AppComponent.h"
#include "AppComponents/Gui.h"
#include "GraphicsEngine.h"
#include "Setup/GraphicsSettings.h"
#include "Setup/VulkanInstance.h"
#include "Time/Time.h"
#include "Window.h"
#include "Texture/RenderTarget.h"

namespace Slipper
{
Application *Application::instance = nullptr;

Application::Application(ApplicationInfo &ApplicationInfo)
{
    ASSERT(instance, "Application allready created!");
    instance = this;

    name = ApplicationInfo.Name;

    glfwInit();
    vulkanInstance = std::make_unique<VulkanInstance>();

    WindowInfo window_create_info;
    window_create_info.width = 1280;
    window_create_info.height = 720;
    window_create_info.name = name.c_str();
    window = std::make_unique<Window>(window_create_info);

    Device::PickPhysicalDevice(&window->GetSurface(), true);
    GraphicsSettings::Get().MSAA_SAMPLES = Device::Get().GetMaxUsableSampleCount();

    GraphicsEngine::Init();
    GraphicsEngine::Get().AddWindow(*window);

    guiComponent = std::make_unique<Gui>("Gui");
    guiComponent->Init();
}

Application::~Application()
{
    vkDeviceWaitIdle(Device::Get());

    guiComponent->Shutdown();

    for (auto &app_component : appComponents) {
        app_component->Shutdown();
    }
    appComponents.clear();
    GraphicsEngine::Shutdown();
    window.reset();
    Device::Destroy();
    glfwTerminate();
}

void Application::AddComponent(AppComponent *ProgramComponent)
{
    ProgramComponent->Init();
    appComponents.push_back(std::unique_ptr<AppComponent>(ProgramComponent));
}

void Application::Close()
{
    running = false;
}

VkImageView Application::GetViewportTextureView() const
{
    return window->GetSurface().swapChain->renderTarget->textureView;
}

void Application::Run()
{
    while (running) {
        window->OnUpdate();

        if (!minimized) {
            Time::Tick(Engine::FRAME_COUNT);
            Engine::FRAME_COUNT += 1;
            if (!(Engine::FRAME_COUNT % 64)) {
                std::cout << '\r' << std::setw(10) << 1.0f / Time::DeltaTimeSmooth() << "fps"
                          << std::setw(2) << Time::DeltaTimeSmooth() << ' ' << std::setw(2) << "ms"
                          << std::flush;
            }

            GraphicsEngine::Get().BeginUpdate();
            for (auto &app_component : appComponents) {
                app_component->OnUpdate();
            }
            guiComponent->OnUpdate();
            GraphicsEngine::Get().EndUpdate();

            GraphicsEngine::Get().BeginGuiUpdate();
            guiComponent->StartNewFrame();
            guiComponent->OnGuiRender();
            for (auto &app_component : appComponents) {
                app_component->OnGuiRender();
            }
            guiComponent->EndNewFrame(GraphicsEngine::Get().GetCurrentCommandBuffer());
            GraphicsEngine::Get().EndGuiUpdate();

            GraphicsEngine::Get().Render();
        }
    }
}

void Application::OnWindowResize(Window *Window, int Width, int Height)
{
    if (Width == 0 || Height == 0) {
        minimized = true;
        return;
    }
    minimized = false;
    GraphicsEngine::OnWindowResized(Window, Width, Height);
}
}  // namespace Slipper