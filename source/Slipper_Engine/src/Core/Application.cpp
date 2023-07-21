#include "Application.h"

#include "AppComponent.h"
#include "AppComponents/Ecs.h"
#include "AppEvents.h"
#include "Event.h"
#include "GraphicsEngine.h"
#include "Input.h"
#include "MaterialManager.h"
#include "Setup/GraphicsSettings.h"
#include "Setup/VulkanInstance.h"
#include "Time/Time.h"
#include "Window.h"

namespace Slipper
{
Application::Application()
{
    instance = this;
}

void Application::Init(ApplicationInfo &ApplicationInfo)
{
    name = ApplicationInfo.Name;

    glfwInit();
    vulkanInstance = new VulkanInstance();

    WindowInfo window_create_info;
    window_create_info.width = 1280;
    window_create_info.height = 720;
    window_create_info.name = name;
    window_create_info.resizable = true;
    window = new Window(window_create_info);
    window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    Device::PickPhysicalDevice(&window->GetSurface(), true);
    GraphicsSettings::Get().MSAA_SAMPLES = Device::Get().GetMaxUsableFramebufferSampleCount();

    // Setup Application Components
    ecsComponent = AddComponent(new Ecs());
    AddComponentBefore(new InputManager(), ecsComponent);
    materialManager = AddComponent(new MaterialManager());

    GraphicsEngine::Init();
    GraphicsEngine::Get().AddWindow(*window);

    AddAdditionalRenderStageUpdate(GraphicsEngine::Get().viewportRenderingStage,
                                   [&](NonOwningPtr<RenderingStage> RS) {
                                       for (const auto &app_component : appComponentsOrdered) {
                                           app_component->OnGuiRender();
                                       }

                                       for (const auto &app_component : appComponentsOrdered) {
                                           app_component->OnUpdate();
                                       }
                                   });
}

void Application::Shutdown()
{
    vkDeviceWaitIdle(Device::Get());

    for (const auto &app_component : appComponents) {
        app_component->Shutdown();
    }
    appComponentsOrdered.clear();
    appComponents.clear();

    GraphicsEngine::Shutdown();
    window.reset();
    Device::Destroy();
    glfwTerminate();
}

void Application::Close()
{
    running = false;
}

void Application::CloseWindow(const Window *Window)
{
    if (Window == window.get()) {
        window.reset();
    }
}

void Application::Run()
{
    while (running) {
        Input::UpdateInputs();
        window->OnUpdate();
        if (!running)
            return;

        if (windowResize.resized) {
            // Do whatever you have to do
        }
        for (auto &[Stage, Info] : viewportsResize) {
            if (Info.resized) {
                ViewportResize(Stage);
            }
        }

        if (!minimized) {
            Time::Tick(Engine::FRAME_COUNT);
            if (!(Engine::FRAME_COUNT % DELTA_SMOOTH_FRAMES)) {
                std::stringstream ss;
                ss << name << " " << std::setw(10) << 1.0f / Time::DeltaTimeSmooth() << "fps  "
                   << std::setw(2) << Time::DeltaTimeSmooth() * 1000 << ' ' << std::setw(2) << "ms"
                   << std::flush;
                window->SetTitle(ss.str());
            }

            // Render loop
            GraphicsEngine::Get().NewFrame();

            // All render stages will be rendered here
            //  That includes the viewport and the window render stage
            //  The Update events are executed via the function added to the viewport stage in
            //  Init()
            for (auto &rendering_stage :
                 GraphicsEngine::Get().renderingStages | std::ranges::views::values) {
                GraphicsEngine::Get().BeginRenderingStage(rendering_stage->name);
                for (auto &stage_update : renderingStagesUpdates[rendering_stage]) {
                    stage_update(rendering_stage.get());
                }
                GraphicsEngine::Get().EndRenderingStage();
            }

            GraphicsEngine::Get().EndFrame();

            Engine::FRAME_COUNT += 1;
        }
    }
}

void Application::OnEvent(Event &Event)
{
    // std::cout << Event.ToString() << '\n';
    switch (Event.GetEventType()) {
        case EventType::WindowClose: {
            WindowCloseEvent &window_close_event = *static_cast<WindowCloseEvent *>(&Event);
            CloseWindow(&window_close_event.window);
            if (!window) {
                Close();
            }
        }
        default:
            for (auto &app_component : appComponents) {
                app_component->OnEvent(Event);
                if (Event.Handled)
                    break;
            };
            break;
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

void Application::OnViewportResize(NonOwningPtr<RenderingStage> Stage,
                                   uint32_t Width,
                                   uint32_t Height)
{
    auto &[context, resized, width, height] = viewportsResize[Stage];
    context = Stage;
    resized = true;
    width = Width;
    height = Height;
}

void Application::AddViewportResizeCallback(
    std::function<void(NonOwningPtr<RenderingStage>, uint32_t, uint32_t)> Callback)
{
    viewportResizeCallbacks.push_back(Callback);
}

void Application::AddAdditionalRenderStageUpdate(
    NonOwningPtr<RenderingStage> Stage,
    std::function<void(NonOwningPtr<RenderingStage>)> UpdateFunction)
{
    renderingStagesUpdates[Stage].emplace_back(UpdateFunction);
}

void Application::AddAdditionalRenderStageUpdateFront(
    NonOwningPtr<RenderingStage> Stage,
    std::function<void(NonOwningPtr<RenderingStage>)> UpdateFunction)
{
    auto &updates = renderingStagesUpdates[Stage];
    updates.emplace(updates.begin(), UpdateFunction);
}

void Application::ViewportResize(NonOwningPtr<RenderingStage> Stage)
{
    vkDeviceWaitIdle(Device::Get());
    auto &[context, resized, width, height] = viewportsResize.at(Stage);
    resized = false;
    GraphicsEngine::OnViewportResize(Stage, width, height);
    for (auto viewport_resize_callback : viewportResizeCallbacks) {
        viewport_resize_callback(Stage, width, height);
    }
}
}  // namespace Slipper