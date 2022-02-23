#include "HelloTriangleApplication.h"

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
    device.CreateSwapChain(&window, &surface);
}

void HelloTriangleApplication::mainLoop()
{
    while (!window.ShouldClose())
    {
        glfwPollEvents();
    }
}

void HelloTriangleApplication::cleanup()
{
    device.Destroy();
    surface.Destroy();
    instance.Destroy();
    window.Destroy();
    glfwTerminate();
}