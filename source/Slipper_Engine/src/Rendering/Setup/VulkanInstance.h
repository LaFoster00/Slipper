#pragma once

namespace Slipper
{
extern VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugUtilsMessengerEXT *pDebugMessenger);

extern void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                          VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks *pAllocator);

extern VkResult CreateDebugReportCallbackEXT(VkInstance instance,
                                             const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugReportCallbackEXT *pCallback);

extern void DestroyDebugReportCallbackEXT(VkInstance instance,
                                          VkDebugReportCallbackEXT callback,
                                          const VkAllocationCallbacks *pAllocator);

class VulkanInstance
{
 public:
    VulkanInstance();

    ~VulkanInstance()
    {
        if (Engine::EnableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
    }

    static VulkanInstance &Get()
    {
        return *m_instance;
    }

    operator VkInstance()
    {
        return instance;
    }

 private:
    static bool CheckValidationLayerSupport();
    static std::vector<const char *> GetRequiredExtensions();

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &CreateInfo);
    void SetupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugUtilsMessages(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                       void *pUserData);

 public:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger = nullptr;

 private:
    static VulkanInstance *m_instance;
};
}  // namespace Slipper