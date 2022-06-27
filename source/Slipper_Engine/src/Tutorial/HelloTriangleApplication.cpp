#include "HelloTriangleApplication.h"

#include "VulkanApp/GraphicsEngine.h"

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
    instance.CreateInstance();
    surface.Create(&instance, window.glfwWindow);
    device = Device::PickPhysicalDevice(&instance, &surface, true);
    graphics = new GraphicsEngine(device);

    auto pipeline = graphics->SetupSimpleRenderPipelineForRenderPass(window, surface, graphics->CreateRenderPass());
    graphics->SetupSimpleDraw();
}

void HelloTriangleApplication::mainLoop()
{
    while (!window.ShouldClose())
    {
        glfwPollEvents();
        graphics->DrawFrame();
    }
}

void HelloTriangleApplication::cleanup()
{
    vkDeviceWaitIdle(device.logicalDevice);

    delete graphics;
    device.Destroy();
    surface.Destroy();
    instance.Destroy();
    window.Destroy();
    glfwTerminate();
}

void HelloTriangleApplication::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
    app->graphics->OnWindowResized(window, width, height);
}
