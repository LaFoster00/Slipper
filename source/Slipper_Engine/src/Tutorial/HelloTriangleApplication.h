#pragma once

#include "Window/Window.h"
#include "VulkanApp/Instance.h"
#include "VulkanApp/Device.h"
#include "VulkanApp/Surface.h"
#include "Engine/Engine.h"
#include "terminal_util.h"
#include "VulkanApp/GraphicsPipeline.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <set>
#include <cstring>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

constexpr bool printCreationDetails = true;

class HelloTriangleApplication
{
public:
    HelloTriangleApplication()
    {
    }

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

private:
    Window window;
    Instance instance;
    Surface surface;
    Device device;
    GraphicsPipeline pipeline;

    VkSwapchainKHR swapchain;

    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    VkDebugUtilsMessengerEXT debugMessenger;
};