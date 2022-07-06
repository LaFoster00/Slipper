#include "Instance.h"

#include <cstring>
#include <iostream>

Instance *Instance::m_instance = nullptr;

Instance::Instance()
{
    ASSERT(Engine::EnableValidationLayers && !CheckValidationLayerSupport(),
           "Validation layers requested, but not available!")

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Slipper";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "SlipperEngine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    uint32_t glfw_extension_count = 0;

    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    if (Engine::EnableValidationLayers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(Engine::VALIDATION_LAYERS.size());
        create_info.ppEnabledLayerNames = Engine::VALIDATION_LAYERS.data();
    }
    else {
        create_info.enabledLayerCount = 0;
    }

    VK_ASSERT(vkCreateInstance(&create_info, nullptr, &instance), "Failed to create VkInstance!")

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    std::cout << "\nRequested Extensions:\n";
    for (const char *extension_name : Engine::DEVICE_EXTENSIONS) {
        std::cout << '\t' << extension_name << '\n';
    }

    std::cout << "\nAvailable Extensions:\n";
    for (const auto & [extension_name, spec_version] : extensions) {
        std::cout << '\t' << extension_name << '\n';
    }
}

bool Instance::CheckValidationLayerSupport()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    std::cout << "Requested Layers:\n";
    for (const char *layer_name : Engine::VALIDATION_LAYERS) {
        std::cout << '\t' << layer_name << '\n';
    }

    std::cout << "\nAvailable Layers:\n";
    for (const auto &layer : available_layers) {
        std::cout << '\t' << layer.layerName << '\n';
    }

    for (const char *layer_name : Engine::VALIDATION_LAYERS) {
        bool layer_found = false;

        for (const auto &layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> Instance::GetRequiredExtensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if (Engine::EnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}