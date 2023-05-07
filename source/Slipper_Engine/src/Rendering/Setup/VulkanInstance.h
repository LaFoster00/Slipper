#pragma once

inline PFN_vkCreateDebugUtilsMessengerEXT PfnVkCreateDebugUtilsMessengerExt;
inline PFN_vkDestroyDebugUtilsMessengerEXT PfnVkDestroyDebugUtilsMessengerExt;

extern VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                               const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                               const VkAllocationCallbacks *pAllocator,
                               VkDebugUtilsMessengerEXT *pMessenger);

extern VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                VkDebugUtilsMessengerEXT messenger,
                                VkAllocationCallbacks const *pAllocator);

extern VkResult CreateDebugReportCallbackEXT(VkInstance instance,
                                             const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugReportCallbackEXT *pCallback);

extern void DestroyDebugReportCallbackEXT(VkInstance instance,
                                          VkDebugReportCallbackEXT callback,
                                          const VkAllocationCallbacks *pAllocator);
namespace Slipper
{

class VulkanInstance
{
 public:
    VulkanInstance();

    ~VulkanInstance()
    {
        if (Engine::EnableValidationLayers) {
            instance.destroyDebugUtilsMessengerEXT(debugMessenger);
        }
        instance.destroy();
    }

    static VulkanInstance &Get()
    {
        return *m_instance;
    }

    static vk::Instance &GetVk()
    {
        return m_instance->instance;
    }

    operator vk::Instance &()
    {
        return instance;
    }

    operator VkInstance() const
    {
        return instance;
    }

 private:
    static bool CheckValidationLayerSupport(std::vector<char const *> const &Layers,
                                            std::vector<vk::LayerProperties> const &Properties);
    static std::vector<const char *> GetRequiredExtensions();

    void SetupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugMessageFunc(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
                     VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
                     const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                     void *pUserData);

 public:
    vk::Instance instance;
    vk::DebugUtilsMessengerEXT debugMessenger;

 private:
    static inline VulkanInstance *m_instance = nullptr;
};
}  // namespace Slipper