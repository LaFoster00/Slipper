#include "HelloTriangleApplication.h"

#include "Rendering/GraphicsEngine.h"

void HelloTriangleApplication::initWindow()
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

void HelloTriangleApplication::initVulkan()
{
    // Creates instance as well as retrieving it
    instance = &Instance::Get();
    surface = new Surface(*window);
    device = Device::PickPhysicalDevice(surface, true);
    surface->CreateSwapChain();
    graphics = new GraphicsEngine(*device);

    auto pipeline = graphics->SetupDebugRender(*surface);
    graphics->SetupSimpleDraw();
}

void HelloTriangleApplication::mainLoop()
{
    while (!window->ShouldClose()) {
        glfwPollEvents();
        graphics->DrawFrame();
    }
}

void HelloTriangleApplication::cleanup()
{
    vkDeviceWaitIdle(*device);

    delete graphics;
    surface->DestroySwapChain();
    Device::Destroy();
    delete surface;
    delete instance;
    delete window;
    glfwTerminate();
}

void HelloTriangleApplication::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    const auto app = static_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
    app->graphics->OnWindowResized(window, width, height);
}
