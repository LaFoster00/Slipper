#pragma once

#include "VulkanApp/GraphicsPipeline/GraphicsPipeline.h"
#include "VulkanApp/Presentation/Surface.h"
#include "VulkanApp/Setup/Device.h"
#include "VulkanApp/Setup/Instance.h"
#include "Window/Window.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

constexpr bool printCreationDetails = true;

class GraphicsEngine;

class HelloTriangleApplication
{
 public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

 private:
#pragma region MainFunctions

    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

#pragma endregion MainFunctions

    static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);

 private:
    Window window;
    Instance instance;
    Surface surface;
    Device *device;
    GraphicsEngine *graphics;

    VkCommandBuffer simpleDrawCommand;

    VkDebugUtilsMessengerEXT debugMessenger;
};