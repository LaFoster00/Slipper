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

    window.CreateWindow(windowCreateInfo);
    glfwSetWindowUserPointer(window.glfwWindow, this);
    glfwSetFramebufferSizeCallback(window.glfwWindow, FramebufferResizeCallback);
}

void HelloTriangleApplication::initVulkan()
{
    // Creates instance as well as retrieving it
    instance = &Instance::Get();
    surface = new Surface(window.glfwWindow);
    device = Device::PickPhysicalDevice(surface, true);
    graphics = new GraphicsEngine(*device);

    auto pipeline = graphics->SetupDebugRender(
        window, *surface);
    graphics->SetupSimpleDraw();
}

void HelloTriangleApplication::mainLoop()
{
    while (!window.ShouldClose()) {
        glfwPollEvents();
        graphics->DrawFrame();
    }
}

void HelloTriangleApplication::cleanup()
{
    vkDeviceWaitIdle(*device);

    delete graphics;
    Device::Destroy();
    delete surface;
    delete instance;
    window.Destroy();
    glfwTerminate();
}

void HelloTriangleApplication::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    const auto app = static_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
    app->graphics->OnWindowResized(window, width, height);
}
