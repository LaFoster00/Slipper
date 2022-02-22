#pragma once

#include "Window/Window.h"
#include "VulkanApp/Instance.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <set>
#include <cstring>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

constexpr bool printCreationDetails = true;
constexpr bool enableValidationLayers = true;

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangleApplication
{
public:
    HelloTriangleApplication() : instance(enableValidationLayers)
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

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapchain;
    VkSurfaceKHR surface;

    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    VkDebugUtilsMessengerEXT debugMessenger;
};