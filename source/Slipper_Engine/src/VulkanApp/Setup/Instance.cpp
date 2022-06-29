#include "Instance.h"

#include <cstring>
#include <iostream>

void Instance::CreateInstance()
{
    ASSERT(Engine::EnableValidationLayers && !CheckValidationLayerSupport(),
           "Validation layers requested, but not available!")

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Slipper";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Slipper_Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    if (Engine::EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(Engine::ValidationLayers.size());
        createInfo.ppEnabledLayerNames = Engine::ValidationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &instance), "Failed to create VkInstance!")

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "\nRequested Extensions:\n";
    for (const char *extensionName : Engine::DeviceExtensions) {
        std::cout << '\t' << extensionName << '\n';
    }

    std::cout << "\nAvailable Extensions:\n";
    for (const auto &extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

bool Instance::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::cout << "Requested Layers:\n";
    for (const char *layerName : Engine::ValidationLayers) {
        std::cout << '\t' << layerName << '\n';
    }

    std::cout << "\nAvailable Layers:\n";
    for (const auto &layer : availableLayers) {
        std::cout << '\t' << layer.layerName << '\n';
    }

    for (const char *layerName : Engine::ValidationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> Instance::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (Engine::EnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}