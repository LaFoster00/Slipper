#include "HelloTriangleApplication.h"

#include "VulkanApp/GraphicsEngine.h"

void HelloTriangleApplication::initWindow()
{
    glfwInit();

    WindowInfo windowCreateInfo;
    windowCreateInfo.name = "Slipper";
    windowCreateInfo.width = WIDTH;
    windowCreateInfo.height = HEIGHT;
    windowCreateInfo.resizable = false;

    window.CreateWindow(windowCreateInfo);
}

void HelloTriangleApplication::initVulkan()
{
    instance.CreateInstance();
    surface.Create(&instance, window.glfwWindow);
    device = Device::PickPhysicalDevice(&instance, &surface, true);
    graphics = new GraphicsEngine(device);

    auto &pipeline = graphics->SetupSimpleRenderPipeline(window, surface);
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