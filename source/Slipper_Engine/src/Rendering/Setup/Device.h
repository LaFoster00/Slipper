#pragma once

#include "Presentation/SwapChain.h"
#include "common_includes.h"
#include <optional>
#include <string>

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
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

// Singleton since there will only be support for a single device
class Device
{
 public:
    Device() = delete;

    static Device &Get()
    {
        return *m_instance;
    }

    static Device &Create(VkPhysicalDevice physicalDevice)
    {
        if (!m_instance)
        {
            m_instance = new Device(physicalDevice);
        }
        return *m_instance;
    }

    static void Destroy()
    {
	    delete m_instance;
    }

    operator VkDevice() const;

    void InitLogicalDevice();

    static Device *PickPhysicalDevice(const Instance *instance,
                                      const Surface *surface,
                                      const bool initLogicalDevice);

    [[nodiscard]] std::string DeviceInfoToString() const;

    void QuerySwapChainSupport(const Surface *surface);

    uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties) const;

 private:
    Device(VkPhysicalDevice physicalDevice);
    ~Device();

    bool IsDeviceSuitable(const Surface *surface);
    uint32_t RateDeviceSuitability() const;
    bool CheckExtensionSupport() const;
    const QueueFamilyIndices *QueryQueueFamilyIndices(const Surface *surface);

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

private:
    static Device *m_instance;
};