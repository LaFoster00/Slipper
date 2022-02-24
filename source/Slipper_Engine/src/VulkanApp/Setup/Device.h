#pragma once

#include "common_includes.h"
#include "../Presentation/SwapChain.h"
#include <string>
#include <optional>

class Instance;
class Surface;
class Window;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Device
{
public:
    Device(){};
    Device(VkPhysicalDevice physicalDevice);
    void Destroy();

    void InitLogicalDevice();
    void CreateSwapChain(Window *window, Surface *Surface);

    static Device PickPhysicalDevice(const Instance *instance, const Surface *surface, const bool initLogicalDevice);

    std::string DeviceInfoToString() const;

private:
    bool IsDeviceSuitable(const Surface *surface);
    uint32_t RateDeviceSuitability() const;
    bool CheckExtensionSupport() const;
    const QueueFamilyIndices *QueryQueueFamilyIndices(const Surface *surface);
    void QuerySwapChainSupport(const Surface *surface);

public:
    /* Gets destroyed toghether with its instance. */
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    QueueFamilyIndices queueFamilyIndices;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    SwapChainSupportDetails swapchainSupportDetails;
};