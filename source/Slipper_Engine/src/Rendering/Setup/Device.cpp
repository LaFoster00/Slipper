#include "Device.h"

#include "Presentation/Surface.h"
#include "VulkanInstance.h"

namespace Slipper
{
Device *Device::m_instance = nullptr;

Device::Device(const vk::PhysicalDevice PhysicalDevice) : physicalDevice(PhysicalDevice)
{
    deviceProperties = physicalDevice.getProperties();
    deviceFeatures = physicalDevice.getFeatures();
}

Device::~Device()
{
    logicalDevice.destroy();
}

void Device::InitLogicalDevice()
{
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {queueFamilyIndices.graphicsFamily.value(),
                                                queueFamilyIndices.presentFamily.value(),
                                                queueFamilyIndices.transferFamily.value()};

    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families) {
        vk::DeviceQueueCreateInfo queue_create_info({}, queue_family, 1, &queue_priority);
        queue_create_infos.push_back(queue_create_info);
    }

    // All features are checked for support during device selection
    vk::PhysicalDeviceFeatures device_features;
    device_features.setSamplerAnisotropy(VK_TRUE);

    std::vector<const char *> enabled_layers;
    if (Engine::EnableValidationLayers)
        enabled_layers = Engine::VALIDATION_LAYERS;

    vk::DeviceCreateInfo create_info(
        {}, queue_create_infos, enabled_layers, Engine::DEVICE_EXTENSIONS, &device_features);

    VK_HPP_ASSERT(physicalDevice.createDevice(&create_info, nullptr, &logicalDevice),
                  "Failed to create logical device")

    graphicsQueue = logicalDevice.getQueue(queueFamilyIndices.graphicsFamily.value(), 0);
    presentQueue = logicalDevice.getQueue(queueFamilyIndices.presentFamily.value(), 0);

    if (queueFamilyIndices.transferFamily.has_value()) {
        logicalDevice.getQueue(queueFamilyIndices.transferFamily.value(), 0, &transferQueue);
    }
}

Device *Device::PickPhysicalDevice(const Surface *Surface, const bool InitLogicalDevice)
{
    std::map<int, Device *, std::greater<>> devices;

    std::cout << "\nFetching all physical devices!\n";

    for (const std::vector<vk::PhysicalDevice> physical_devices =
             VulkanInstance::GetVk().enumeratePhysicalDevices();
         const auto &physical_device : physical_devices) {
        if (auto device = new Device(physical_device); device->IsDeviceSuitable(Surface)) {
            LOG(device->DeviceInfoToString());
            devices.insert(std::make_pair(device->RateDeviceSuitability(), device));
        }
    }

    ASSERT(!devices.empty(), "Failed to find suitable GPU!");

    if (InitLogicalDevice && !devices.empty()) {
        devices.begin()->second->InitLogicalDevice();
    }

    // Safe final device and delete the rest
    m_instance = devices.begin()->second;
    devices.erase(devices.begin()->first);

    for (const auto new_device : devices | std::ranges::views::values) {
        delete new_device;
    }

    return m_instance;
}

std::string Device::DeviceInfoToString() const
{

    std::string info;
    info += "\nPhysical Device: ";
    info += deviceProperties.deviceName.data();
    info += " ";
    info += vk::to_string(deviceProperties.deviceType);

    info += "\n\t Vendor ID: ";
    info += std::to_string(deviceProperties.vendorID);

    info += "\n\t Driver Version: ";
    info += std::to_string(deviceProperties.driverVersion);

    info += "\n\n";

    return info;
}

vk::SurfaceCapabilitiesKHR Device::GetPhysicalDeviceSurfaceCapabilities(
    const Surface *Surface) const
{
    return physicalDevice.getSurfaceCapabilitiesKHR(Surface->vkSurface);
}

uint32_t Device::SurfaceSwapChainImagesCount(const Surface *Surface) const
{
    const auto swap_chain_support = QuerySwapChainSupport(Surface);
    return CapabilitiesSwapChainImageCount(swap_chain_support);
}

uint32_t Device::CapabilitiesSwapChainImageCount(const SwapChainSupportDetails &Support)
{
    return std::clamp(static_cast<uint32_t>(Support.capabilities.minImageCount),
                      static_cast<uint32_t>(0),
                      static_cast<uint32_t>(std::max(Support.capabilities.maxImageCount,
                                                     Support.capabilities.minImageCount)));
}

SwapChainSupportDetails Device::QuerySwapChainSupport(const Surface *Surface) const
{
    SwapChainSupportDetails swap_chain_support_details;
    swap_chain_support_details.capabilities = GetPhysicalDeviceSurfaceCapabilities(Surface);
    swap_chain_support_details.formats = physicalDevice.getSurfaceFormatsKHR(Surface->vkSurface);
    swap_chain_support_details.presentModes = physicalDevice.getSurfacePresentModesKHR(
        Surface->vkSurface);
    return swap_chain_support_details;
}

VkExtent2D Device::GetSurfaceResolution(const Surface *Surface) const
{
    return GetPhysicalDeviceSurfaceCapabilities(Surface).currentExtent;
}

vk::SampleCountFlagBits Device::GetMaxUsableFramebufferSampleCount() const
{
    const vk::SampleCountFlags counts = deviceProperties.limits.framebufferColorSampleCounts &
                                        deviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64) {
        return vk::SampleCountFlagBits::e64;
    }
    if (counts & vk::SampleCountFlagBits::e32) {
        return vk::SampleCountFlagBits::e32;
    }
    if (counts & vk::SampleCountFlagBits::e16) {
        return vk::SampleCountFlagBits::e16;
    }
    if (counts & vk::SampleCountFlagBits::e8) {
        return vk::SampleCountFlagBits::e8;
    }
    if (counts & vk::SampleCountFlagBits::e4) {
        return vk::SampleCountFlagBits::e4;
    }
    if (counts & vk::SampleCountFlagBits::e2) {
        return vk::SampleCountFlagBits::e2;
    }
    return vk::SampleCountFlagBits::e1;
}

uint32_t Device::FindMemoryType(const uint32_t TypeFilter,
                                const vk::MemoryPropertyFlags Properties) const
{
    const vk::PhysicalDeviceMemoryProperties mem_properties = physicalDevice.getMemoryProperties();

    for (uint32_t memory_type = 0; memory_type < mem_properties.memoryTypeCount; memory_type++) {
        if (TypeFilter & (1 << memory_type) &&
            (mem_properties.memoryTypes[memory_type].propertyFlags & Properties) == Properties) {
            return memory_type;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

bool Device::IsDeviceSuitable(const Surface *Surface)
{
    const auto [capabilities, formats, presentModes] = QuerySwapChainSupport(Surface);
    /* Populate the queue family indices. */
    if (const bool device_type_suitable = deviceProperties.deviceType ==
                                              vk::PhysicalDeviceType::eDiscreteGpu ||
                                          deviceProperties.deviceType ==
                                              vk::PhysicalDeviceType::eIntegratedGpu;
        !device_type_suitable)
        return false;

    QueryQueueFamilyIndices(Surface);
    if (!queueFamilyIndices.IsComplete())
        return false;

    if (!CheckExtensionSupport() || !CheckFeatureSupport())
        return false;

    return !formats.empty() && !presentModes.empty();
}

uint32_t Device::RateDeviceSuitability() const
{
    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

bool Device::CheckExtensionSupport() const
{
    std::vector<vk::ExtensionProperties> available_extensions =
        physicalDevice.enumerateDeviceExtensionProperties();

    std::set<std::string> required_extensions(Engine::DEVICE_EXTENSIONS.begin(),
                                              Engine::DEVICE_EXTENSIONS.end());

    for (const auto &[extension_name, spec_version] : available_extensions) {
        required_extensions.erase(extension_name);
    }

    return required_extensions.empty();
}

bool Device::CheckFeatureSupport() const
{
	const vk::PhysicalDeviceFeatures supported_features = physicalDevice.getFeatures();

    return supported_features.samplerAnisotropy && supported_features.geometryShader;
}

const QueueFamilyIndices *Device::QueryQueueFamilyIndices(const Surface *Surface)
{
    if (queueFamilyIndices.IsComplete())
        return &queueFamilyIndices;

    QueueFamilyIndices indices;


    const std::vector<vk::QueueFamilyProperties> queue_families = physicalDevice.getQueueFamilyProperties();

    int graphics_queue_family_index = 0;
    for (const auto &queue_family : queue_families) {
        if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = graphics_queue_family_index;
            break;
        }

        graphics_queue_family_index++;
    }

    if (physicalDevice.getSurfaceSupportKHR(graphics_queue_family_index, *Surface)) {
        indices.presentFamily = graphics_queue_family_index;
    }

    graphics_queue_family_index = 0;
    for (const auto &queue_family : queue_families) {
        if ((queue_family.queueFlags & vk::QueueFlagBits::eGraphics) &&
            queue_family.queueFlags & vk::QueueFlagBits::eTransfer) {
            indices.transferFamily = graphics_queue_family_index;
            break;
        }

        ++graphics_queue_family_index;
    }

    queueFamilyIndices = indices;
    return &queueFamilyIndices;
}
}  // namespace Slipper