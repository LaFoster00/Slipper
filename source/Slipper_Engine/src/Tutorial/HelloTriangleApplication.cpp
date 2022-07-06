#include "HelloTriangleApplication.h"

#include "Rendering/GraphicsEngine.h"

void HelloTriangleApplication::InitWindow()
{
    glfwInit();

    WindowInfo windowCreateInfo;
    windowCreateInfo.name = "Slipper";
    windowCreateInfo.width = WIDTH;
    windowCreateInfo.height = HEIGHT;
    windowCreateInfo.resizable = true;

    window = new Window(windowCreateInfo);
    glfwSetWindowUserPointer(*window, this);
    glfwSetFramebufferSizeCallback(*window, FramebufferResizeCallback);
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

void HelloTriangleApplication::FramebufferResizeCallback(GLFWwindow *Window, int Width, int Height)
{
    const auto app = static_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(Window));
    app->graphics->OnWindowResized();
}
