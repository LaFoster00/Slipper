#pragma once

namespace Slipper
{
class VulkanInstance;
class Surface;
class Window;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;

    bool IsComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value() &&
               transferFamily.has_value();
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

    static void Destroy()
    {
        delete m_instance;
    }

    operator VkDevice() const
    {
        return logicalDevice;
    }

    operator VkPhysicalDevice() const
    {
        return physicalDevice;
    }

    void InitLogicalDevice();

    static Device *PickPhysicalDevice(const Surface *Surface, bool InitLogicalDevice);

    [[nodiscard]] std::string DeviceInfoToString() const;

    VkSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilities(const Surface *Surface) const;
    uint32_t SurfaceSwapChainImagesCount(const Surface *Surface) const;
    static uint32_t CapabilitiesSwapChainImageCount(const SwapChainSupportDetails &Support);
    SwapChainSupportDetails QuerySwapChainSupport(const Surface *Surface) const;
    VkExtent2D GetSurfaceResolution(const Surface *Surface) const;
    VkSampleCountFlagBits GetMaxUsableSampleCount() const;

    uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties) const;

 private:
    Device(VkPhysicalDevice PhysicalDevice);
    ~Device();

    bool IsDeviceSuitable(const Surface *Surface);
    uint32_t RateDeviceSuitability() const;
    bool CheckExtensionSupport() const;
    bool CheckFeatureSupport() const;
    const QueueFamilyIndices *QueryQueueFamilyIndices(const Surface *Surface);

 public:
    /* Gets destroyed toghether with its instance. */
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    QueueFamilyIndices queueFamilyIndices;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkQueue transferQueue = VK_NULL_HANDLE;

 private:
    static Device *m_instance;
};
}  // namespace Slipper