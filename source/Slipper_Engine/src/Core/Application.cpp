#include "Application.h"

#include "AppComponent.h"
#include "AppComponents/Gui.h"
#include "AppEvents.h"
#include "Event.h"
#include "GraphicsEngine.h"
#include "Setup/GraphicsSettings.h"
#include "Setup/VulkanInstance.h"
#include "Time/Time.h"
#include "Window.h"

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
    window_create_info.name = name;
    window = std::make_unique<Window>(window_create_info);
    window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

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

void Application::CloseWindow(Window *Window)
{
    if (Window == window.get()) {
        window.reset();
    }
}

void Application::Run()
{
    while (running) {
        window->OnUpdate();
        if (windowResize.resized) {
            WindowResize();
        }
        if (viewportResize.resized) {
            ViewportResize();
        }

        if (!running)
            return;

        if (!minimized) {
            Time::Tick(Engine::FRAME_COUNT);
            Engine::FRAME_COUNT += 1;
            if (!(Engine::FRAME_COUNT % DELTA_SMOOTH_FRAMES)) {
                std::stringstream ss;
                ss << name << " " << std::setw(10) << 1.0f / Time::DeltaTimeSmooth() << "fps  "
                   << std::setw(2) << Time::DeltaTimeSmooth() << ' ' << std::setw(2) << "ms"
                   << std::flush;
                window->SetTitle(ss.str());
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
            guiComponent->EndNewFrame(GraphicsEngine::Get().GetCurrentGuiCommandBuffer());
            GraphicsEngine::Get().EndGuiUpdate();

            GraphicsEngine::Get().Render();
        }
    }
}

void Application::OnEvent(Event &Event)
{
    //std::cout << Event.ToString() << '\n';
    switch (Event.GetEventType()) {
        case EventType::WindowClose: {
            WindowCloseEvent &window_close_event = *static_cast<WindowCloseEvent *>(&Event);
            CloseWindow(&window_close_event.window);
            if (!window) {
                Close();
            }
        }
    default: ;
    }
}

void Application::OnWindowResize(Window *Window, int Width, int Height)
{
    if (Width == 0 || Height == 0) {
        minimized = true;
        return;
    }
    minimized = false;

    windowResize.context = Window;
    windowResize.resized = true;
    windowResize.width = Width;
    windowResize.height = Height;
}

void Application::OnViewportResize(uint32_t Width, uint32_t Height)
{
    viewportResize.resized = true;
    viewportResize.width = Width;
    viewportResize.height = Height;
}

void Application::AddViewportResizeCallback(std::function<void(uint32_t, uint32_t)> Callback)
{
    viewportResizeCallbacks.push_back(Callback);
}

void Application::WindowResize()
{
    windowResize.resized = false;
    GraphicsEngine::OnWindowResized(
        std::any_cast<Window *>(windowResize.context), windowResize.width, windowResize.height);
}

void Application::ViewportResize()
{
    viewportResize.resized = false;
    GraphicsEngine::OnViewportResize(viewportResize.width, viewportResize.height);
    for (auto viewport_resize_callback : viewportResizeCallbacks) {
        viewport_resize_callback(viewportResize.width, viewportResize.height);
    }
}
}  // namespace Slipper