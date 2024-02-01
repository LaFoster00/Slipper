#include "VulkanInstance.h"

#include <cstring>
#include <iostream>

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                               const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                               const VkAllocationCallbacks *pAllocator,
                               VkDebugUtilsMessengerEXT *pMessenger)
{
    return PfnVkCreateDebugUtilsMessengerExt(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                           VkDebugUtilsMessengerEXT messenger,
                                                           VkAllocationCallbacks const *pAllocator)
{
    return PfnVkDestroyDebugUtilsMessengerExt(instance, messenger, pAllocator);
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

namespace Slipper
{
VulkanInstance::VulkanInstance()
{

    ASSERT(!m_instance, "Instance allready created!");
    m_instance = this;

    constexpr vk::ApplicationInfo app_info("Slipper",
                                           VK_MAKE_VERSION(1, 0, 0),
                                           "SlipperEngine",
                                           VK_MAKE_VERSION(1, 0, 0),
                                           VK_API_VERSION_1_3);

    std::vector<const char *> validation_layers;
    vk::ValidationFeaturesEXT validation_features;
    if (Engine::EnableValidationLayers) {
        const std::vector instance_layer_properties = vk::enumerateInstanceLayerProperties();

        ASSERT(
            Engine::EnableValidationLayers &&
                CheckValidationLayerSupport(Engine::VALIDATION_LAYERS, instance_layer_properties),
            "Validation layers requested, but not available!")
        validation_layers = Engine::VALIDATION_LAYERS;

        validation_features.setEnabledValidationFeatures(Engine::PRINTF_ENABLES);
    }

    // Extensions
    auto extensions = GetRequiredExtensions();
    extensions.insert(
        extensions.end(), Engine::INSTANCE_EXTENSIONS.begin(), Engine::INSTANCE_EXTENSIONS.end());
    std::cout << "\nRequested Instance Extensions:\n";
    for (const char *extension_name : extensions) {
        std::cout << '\t' << extension_name << '\n';
    }

    std::vector extension_properties = vk::enumerateInstanceExtensionProperties(nullptr);

    std::cout << "\nAvailable Instance Extensions:\n";
    for (const auto &[extension_name, spec_version] : extension_properties) {

        std::cout << std::format("\t{} V{}", extension_name.data(), spec_version) << '\n';
    }

    const vk::InstanceCreateInfo create_info(
        vk::InstanceCreateFlags(), &app_info, validation_layers, extensions, &validation_features);

    VK_HPP_ASSERT(vk::createInstance(&create_info, nullptr, &instance),
                  "Failed to create VkInstance!")

    // Setup Debug Messenger
    PfnVkCreateDebugUtilsMessengerExt = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!PfnVkCreateDebugUtilsMessengerExt) {
        std::cout
            << "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function."
            << std::endl;
        exit(1);
    }

    PfnVkDestroyDebugUtilsMessengerExt = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!PfnVkDestroyDebugUtilsMessengerExt) {
        std::cout
            << "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function."
            << std::endl;
        exit(1);
    }

    SetupDebugMessenger();
}

bool VulkanInstance::CheckValidationLayerSupport(
    std::vector<char const *> const &Layers, std::vector<vk::LayerProperties> const &Properties)
{
    std::cout << "Requested Layers:\n";
    for (const char *layer_name : Engine::VALIDATION_LAYERS) {
        std::cout << '\t' << layer_name << '\n';
    }

    std::cout << "\nAvailable Layers:\n";
    for (const auto &layer : Layers) {
        std::cout << '\t' << layer << '\n';
    }

    // return true if all layers are listed in the properties
    return std::ranges::all_of(Layers, [&Properties](char const *name) {
        return std::ranges::find_if(Properties, [&name](vk::LayerProperties const &property) {
                   return strcmp(property.layerName, name) == 0;
               }) != Properties.end();
    });
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

void VulkanInstance::SetupDebugMessenger()
{
    if (!Engine::EnableValidationLayers)
        return;

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
#ifdef SHADER_PRINTF_ENABLED
    severityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
#endif

    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    debugMessenger = instance.createDebugUtilsMessengerEXT(vk::DebugUtilsMessengerCreateInfoEXT(
        {}, severityFlags, messageTypeFlags, &VulkanInstance::DebugMessageFunc));
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanInstance::DebugMessageFunc(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
                                 VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
                                 const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                 void *pUserData)
{

    std::string message;

    message += vk::to_string(
                   static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(MessageSeverity)) +
               ": " + vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(MessageTypes)) +
               ":\n";
    message += std::string("\t") + "messageIDName   = <" + pCallbackData->pMessageIdName + ">\n";
    message += std::string("\t") +
               "messageIdNumber = " + std::to_string(pCallbackData->messageIdNumber) + "\n";
    message += std::string("\t") + "message         = <" + pCallbackData->pMessage + ">\n";
    if (0 < pCallbackData->queueLabelCount) {
        message += std::string("\t") + "Queue Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++) {
            message += std::string("\t\t") + "labelName = <" +
                       pCallbackData->pQueueLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount) {
        message += std::string("\t") + "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
            message += std::string("\t\t") + "labelName = <" +
                       pCallbackData->pCmdBufLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->objectCount) {
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
            message += std::string("\t") + "Object " + std::to_string(i) + "\n";
            message += std::string("\t\t") + "objectType   = " +
                       vk::to_string(
                           static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) +
                       "\n";
            message += std::string("\t\t") + "objectHandle = " +
                       std::to_string(pCallbackData->pObjects[i].objectHandle) + "\n";
            if (pCallbackData->pObjects[i].pObjectName) {
                message += std::string("\t\t") + "objectName   = <" +
                           pCallbackData->pObjects[i].pObjectName + ">\n";
            }
        }
    }

    std::cout << message << std::endl;

    return false;
}
}  // namespace Slipper