#pragma once

#include "common_defines.h"
#include "common_includes.h"
#include <vector>

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

class Instance
{
 public:
    Instance();

    ~Instance()
    {
        if (Engine::EnableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
    }

    static Instance &Get()
    {
        if (!m_instance) {
            m_instance = new Instance();
        }
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
    static Instance *m_instance;
};