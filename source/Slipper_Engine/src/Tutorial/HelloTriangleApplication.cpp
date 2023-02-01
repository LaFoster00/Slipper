#include "HelloTriangleApplication.h"

#include "Rendering/GraphicsEngine.h"
#include "Time/Time.h"

void HelloTriangleApplication::InitWindow()
{
    glfwInit();

    WindowInfo windowCreateInfo;
    windowCreateInfo.name = "Slipper";
    windowCreateInfo.width = WIDTH;
    windowCreateInfo.height = HEIGHT;
    windowCreateInfo.resizable = true;

    window = new Window(windowCreateInfo);
}

void HelloTriangleApplication::InitVulkan()
{
    // Creates instance as well as retrieving it
    instance = &Instance::Get();
    surface = new Surface(*window);
    device = Device::PickPhysicalDevice(surface, true);
    surface->CreateSwapChain();
    graphics = &GraphicsEngine::Get();

    graphics->SetupDebugRender(*surface);
}

void HelloTriangleApplication::MainLoop()
{
    while (!window->ShouldClose()) {
        glfwPollEvents();
        Time::Tick(Engine::FRAME_COUNT);
        Engine::FRAME_COUNT += 1;
        if (!(Engine::FRAME_COUNT % 64)) {
            std::cout << '\r' << std::setw(10) << 1.0f / Time::DeltaTimeSmooth() << "fps" << std::setw(2)
                      << Time::DeltaTimeSmooth() << ' ' << std::setw(2) << "ms" << std::flush;
        }
        graphics->DrawFrame();
    }
}

void HelloTriangleApplication::Cleanup()
{
    vkDeviceWaitIdle(*device);

    GraphicsEngine::Destroy();
    surface->DestroyDeviceDependencies();
    Device::Destroy();
    delete surface;
    delete instance;
    delete window;
    glfwTerminate();
}
