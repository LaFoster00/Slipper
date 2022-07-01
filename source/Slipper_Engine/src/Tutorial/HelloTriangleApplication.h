#pragma once

#include "Rendering/GraphicsPipeline/GraphicsPipeline.h"
#include "Rendering/Presentation/Surface.h"
#include "Rendering/Setup/Device.h"
#include "Rendering/Setup/Instance.h"
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
    Window *window;
    Instance *instance = nullptr;
    Surface *surface = nullptr;
    Device *device = nullptr;
    GraphicsEngine *graphics = nullptr;

    VkCommandBuffer simpleDrawCommand = nullptr;

    VkDebugUtilsMessengerEXT debugMessenger = nullptr;
};