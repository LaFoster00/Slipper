#pragma once

#include "common_defines.h"
#include "common_includes.h"
#include <vector>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

class Instance
{
public:
    Instance() = delete;
    Instance(bool enableValidationLayers)
    {
        m_enableValidationLayers = enableValidationLayers;
    }

    void CreateInstance();

    void Destroy()
    {
        vkDestroyInstance(instance, nullptr);
    }

private:
    bool CheckValidationLayerSupport();
    std::vector<const char *> GetRequiredExtensions();

public:
    VkInstance instance;
    std::vector<VkExtensionProperties> extensions;

private:
    bool m_enableValidationLayers;
};