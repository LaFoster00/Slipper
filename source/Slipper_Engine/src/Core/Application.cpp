#include "Application.h"

#include "GraphicsEngine.h"
#include "Window.h"
#include "Setup/GraphicsSettings.h"
#include "Setup/VulkanInstance.h"
#include "Time/Time.h"

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
    GraphicsEngine::Get().SetupDebugRender(window->GetSurface());
}

Application::~Application()
{
    GraphicsEngine::Shutdown();
    Device::Destroy();
    window.reset();
    glfwTerminate();
}

void Application::AddProgramComponent(ProgramComponent* ProgramComponent)
{
}

void Application::Close()
{
    running = false;
}

void Application::Run()
{
    while (running) {
        window->OnUpdate();
        Time::Tick(Engine::FRAME_COUNT);
        Engine::FRAME_COUNT += 1;
        if (!(Engine::FRAME_COUNT % 64)) {
            std::cout << '\r' << std::setw(10) << 1.0f / Time::DeltaTimeSmooth() << "fps"
                      << std::setw(2) << Time::DeltaTimeSmooth() << ' ' << std::setw(2) << "ms"
                      << std::flush;
        }
        GraphicsEngine::Get().DrawFrame();
    }
}
}  // namespace Slipper