#include "Device.h"

#include "Presentation/Surface.h"
#include "VulkanInstance.h"
#include "Window/Window.h"

namespace Slipper
{
Device *Device::m_instance = nullptr;

Device::Device(const VkPhysicalDevice PhysicalDevice) : physicalDevice(PhysicalDevice)
{
    vkGetPhysicalDeviceProperties(PhysicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(PhysicalDevice, &deviceFeatures);
}

Device::~Device()
{
    vkDestroyDevice(logicalDevice, nullptr);
}

void Device::InitLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {queueFamilyIndices.graphicsFamily.value(),
                                                queueFamilyIndices.presentFamily.value(),
                                                queueFamilyIndices.transferFamily.value()};

    float queue_priority = 1.0f;
    for (uint32_t queueFamily : unique_queue_families) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;

        queueCreateInfo.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queueCreateInfo);
    }

    // All features are checked for support during device selection
    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    create_info.pEnabledFeatures = &device_features;

    create_info.enabledExtensionCount = static_cast<uint32_t>(Engine::DEVICE_EXTENSIONS.size());
    create_info.ppEnabledExtensionNames = Engine::DEVICE_EXTENSIONS.data();

    if (Engine::EnableValidationLayers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(Engine::VALIDATION_LAYERS.size());
        create_info.ppEnabledLayerNames = Engine::VALIDATION_LAYERS.data();
    }
    else {
        create_info.enabledLayerCount = 0;
    }

    VK_ASSERT(vkCreateDevice(physicalDevice, &create_info, nullptr, &logicalDevice),
              "Failed to create logical device")

    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
    if (queueFamilyIndices.transferFamily.has_value()) {
        vkGetDeviceQueue(
            logicalDevice, queueFamilyIndices.transferFamily.value(), 0, &transferQueue);
    }
}

Device *Device::PickPhysicalDevice(const Surface *Surface, const bool InitLogicalDevice)
{
    static std::map<int, Device *, std::greater<>> devices;

    if (devices.empty()) {
        std::cout << "\nFetching all physical devices!\n";

        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(VulkanInstance::Get().instance, &device_count, nullptr);
        if (device_count == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> all_physical_devices(device_count);
        vkEnumeratePhysicalDevices(
            VulkanInstance::Get().instance, &device_count, all_physical_devices.data());

        for (const auto &physical_device : all_physical_devices) {
            if (auto device = new Device(physical_device); device->IsDeviceSuitable(Surface)) {
                LOG(device->DeviceInfoToString());
                devices.insert(std::make_pair(device->RateDeviceSuitability(), device));
            }
        }
    }

    ASSERT(!devices.empty(), "Failed to find suitable GPU!");

    if (InitLogicalDevice && devices.begin()->second->logicalDevice == VK_NULL_HANDLE) {
        devices.begin()->second->InitLogicalDevice();
    }

    for (auto deviceIter = ++devices.begin(); deviceIter != devices.end(); ++deviceIter) {
        delete deviceIter->second;
    }

    m_instance = devices.begin()->second;
    return devices.begin()->second;
}

std::string Device::DeviceInfoToString() const
{
    std::string info;
    info += "\nPhysical Device: ";
    info += deviceProperties.deviceName;
    info += " ";
    info += deviceProperties.deviceType;

    info += "\n\t Vendor ID: ";
    info += std::to_string(deviceProperties.vendorID);

    info += "\n\t Driver Version: ";
    info += std::to_string(deviceProperties.driverVersion);

    info += "\n\n";

    return info;
}

VkSurfaceCapabilitiesKHR Device::GetPhysicalDeviceSurfaceCapabilities(const Surface *Surface) const
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, Surface->vkSurface, &capabilities);
    return capabilities;
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
    SwapChainSupportDetails swap_chain_support_details{};

    swap_chain_support_details.capabilities = GetPhysicalDeviceSurfaceCapabilities(Surface);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice, Surface->vkSurface, &format_count, nullptr);

    if (format_count != 0) {
        swap_chain_support_details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                             Surface->vkSurface,
                                             &format_count,
                                             swap_chain_support_details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice, Surface->vkSurface, &present_mode_count, nullptr);

    if (present_mode_count != 0) {
        swap_chain_support_details.presentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                                  Surface->vkSurface,
                                                  &present_mode_count,
                                                  swap_chain_support_details.presentModes.data());
    }

    return swap_chain_support_details;
}

VkExtent2D Device::GetSurfaceResolution(const Surface *Surface) const
{
    return GetPhysicalDeviceSurfaceCapabilities(Surface).currentExtent;
}

VkSampleCountFlagBits Device::GetMaxUsableSampleCount() const
{
    const VkSampleCountFlags counts = deviceProperties.limits.framebufferColorSampleCounts &
                                      deviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT) {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT) {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT) {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT) {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT) {
        return VK_SAMPLE_COUNT_2_BIT;
    }
    return VK_SAMPLE_COUNT_1_BIT;
}

uint32_t Device::FindMemoryType(const uint32_t TypeFilter,
                                const VkMemoryPropertyFlags Properties) const
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem_properties);

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
                                              VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
                                          deviceProperties.deviceType ==
                                              VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
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
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
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
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(
        physicalDevice, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(Engine::DEVICE_EXTENSIONS.begin(),
                                              Engine::DEVICE_EXTENSIONS.end());

    for (const auto &[extension_name, spec_version] : available_extensions) {
        required_extensions.erase(extension_name);
    }

    return required_extensions.empty();
}

bool Device::CheckFeatureSupport() const
{
    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &supported_features);

    return supported_features.samplerAnisotropy && supported_features.geometryShader;
}

const QueueFamilyIndices *Device::QueryQueueFamilyIndices(const Surface *Surface)
{
    if (queueFamilyIndices.IsComplete())
        return &queueFamilyIndices;

    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queue_family_count, queueFamilies.data());

    int i = 0;
    for (const auto &queue_family : queueFamilies) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            break;
        }

        i++;
    }

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, Surface->vkSurface, &present_support);
    if (present_support) {
        indices.presentFamily = i;
    }

    i = 0;
    for (const auto &queue_family : queueFamilies) {
        if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transferFamily = i;
            break;
        }

        ++i;
    }

    queueFamilyIndices = indices;
    return &queueFamilyIndices;
}
}  // namespace Slipper