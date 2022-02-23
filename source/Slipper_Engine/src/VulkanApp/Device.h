#pragma once

#include "common_includes.h"
#include <string>
#include <optional>

class Instance;
class Surface;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class Device
{
public:
    Device(){};
    Device(VkPhysicalDevice physicalDevice);

    void InitLogicalDevice();
    inline void Destroy()
    {
        vkDestroyDevice(logicalDevice, nullptr);
    }

    std::string DeviceInfoToString() const;

    static Device PickPhysicalDevice(const Instance *instance, const Surface *surface);
    const QueueFamilyIndices *PopulateQueueFamilyIndices(const Surface *surface);

private:
    bool IsDeviceSuitable(const Surface *surface);
    uint32_t RateDeviceSuitability() const;

public:
    /* Gets destroyed toghether with its instance. */
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    QueueFamilyIndices queueFamilyIndices;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
};