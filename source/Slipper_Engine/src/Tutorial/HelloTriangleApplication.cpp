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
    instance.Destroy();
    window.Destroy();
    glfwTerminate();
}