#pragma once

#include "common_defines.h"
#include "common_includes.h"
#include <vector>

class Instance
{
public:
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
};