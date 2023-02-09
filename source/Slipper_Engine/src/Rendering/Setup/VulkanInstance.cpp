#include "VulkanInstance.h"

#include <cstring>
#include <iostream>

namespace Slipper
{
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkResult CreateDebugReportCallbackEXT(VkInstance instance,
                                      const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugReportCallbackEXT *pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(VkInstance instance,
                                   VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

VulkanInstance *VulkanInstance::m_instance = nullptr;

VulkanInstance::VulkanInstance()
{
    ASSERT(m_instance, "Instance allready created!");
    m_instance = this;

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

    auto required_extensions = GetRequiredExtensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    VkValidationFeaturesEXT validation_features_ext{};
    if (Engine::EnableValidationLayers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(Engine::VALIDATION_LAYERS.size());
        create_info.ppEnabledLayerNames = Engine::VALIDATION_LAYERS.data();

        PopulateDebugMessengerCreateInfo(debug_create_info);
        create_info.pNext = &debug_create_info;

        validation_features_ext.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        validation_features_ext.enabledValidationFeatureCount = static_cast<uint32_t>(
            Engine::PRINTF_ENABLES.size());
        validation_features_ext.pEnabledValidationFeatures = Engine::PRINTF_ENABLES.data();
        validation_features_ext.pNext = create_info.pNext;

        create_info.pNext = &validation_features_ext;
    }
    else {
        create_info.enabledLayerCount = 0;
    }

    VK_ASSERT(vkCreateInstance(&create_info, nullptr, &instance), "Failed to create VkInstance!")

    SetupDebugMessenger();

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    std::cout << "\nRequested Extensions:\n";
    for (const char *extension_name : Engine::DEVICE_EXTENSIONS) {
        std::cout << '\t' << extension_name << '\n';
    }

    std::cout << "\nAvailable Extensions:\n";
    for (const auto &[extension_name, spec_version] : extensions) {
        std::cout << '\t' << extension_name << '\n';
    }
}

bool VulkanInstance::CheckValidationLayerSupport()
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

std::vector<const char *> VulkanInstance::GetRequiredExtensions()
{
    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if (Engine::EnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanInstance::PopulateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &CreateInfo)
{
    CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

#ifdef SHADER_PRINTF_ENABLED
    CreateInfo.messageSeverity = CreateInfo.messageSeverity |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
#endif
    CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    CreateInfo.pfnUserCallback = DebugUtilsMessages;
    CreateInfo.pUserData = nullptr;  // Optional
}

void VulkanInstance::SetupDebugMessenger()
{
    if (!Engine::EnableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT utils_messenger_create_info{};
    PopulateDebugMessengerCreateInfo(utils_messenger_create_info);

    CreateDebugUtilsMessengerEXT(instance, &utils_messenger_create_info, nullptr, &debugMessenger);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanInstance::DebugUtilsMessages(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                   void *pUserData)
{

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
}  // namespace Slipper