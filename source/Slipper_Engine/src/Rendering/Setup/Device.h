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
	vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
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

    static vk::Device &GetVk()
    {
        return m_instance->logicalDevice;
    }

    static void Destroy()
    {
        delete m_instance;
    }

    operator vk::Device &()
    {
        return logicalDevice;
    }

    operator VkDevice() const
    {
        return logicalDevice;
    }

    operator vk::PhysicalDevice &()
    {
        return physicalDevice;
    }

    operator VkPhysicalDevice() const
    {
        return physicalDevice;
    }

    void InitLogicalDevice();

    static Device *PickPhysicalDevice(const Surface *Surface, bool InitLogicalDevice);

    [[nodiscard]] std::string DeviceInfoToString() const;

    vk::SurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilities(const Surface *Surface) const;
    uint32_t SurfaceSwapChainImagesCount(const Surface *Surface) const;
    static uint32_t CapabilitiesSwapChainImageCount(const SwapChainSupportDetails &Support);
    SwapChainSupportDetails QuerySwapChainSupport(const Surface *Surface) const;
    VkExtent2D GetSurfaceResolution(const Surface *Surface) const;
    vk::SampleCountFlagBits GetMaxUsableFramebufferSampleCount() const;

    uint32_t FindMemoryType(uint32_t TypeFilter, vk::MemoryPropertyFlags Properties) const;

 private:
    Device(vk::PhysicalDevice PhysicalDevice);
    ~Device();

    bool IsDeviceSuitable(const Surface *Surface);
    uint32_t RateDeviceSuitability() const;
    bool CheckExtensionSupport() const;
    bool CheckFeatureSupport() const;
    const QueueFamilyIndices *QueryQueueFamilyIndices(const Surface *Surface);

 public:
    /* Gets destroyed toghether with its instance. */
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;

    vk::PhysicalDeviceProperties deviceProperties;
    vk::PhysicalDeviceFeatures deviceFeatures;

    QueueFamilyIndices queueFamilyIndices;

    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::Queue transferQueue;

 private:
    static Device *m_instance;
};
}  // namespace Slipper