#include "Device.h"
#include "../Presentation/Surface.h"
#include "Instance.h"
#include "Window/Window.h"

#include <iostream>
#include <map>
#include <set>

Device *Device::m_instance = nullptr;

Device::Device(VkPhysicalDevice physicalDevice) : physicalDevice(physicalDevice)
{
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
}

Device::~Device()
{
    vkDestroyDevice(logicalDevice, nullptr);
}

Device::operator VkDevice() const
{
    return logicalDevice;
}

void Device::InitLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {queueFamilyIndices.graphicsFamily.value(),
                                              queueFamilyIndices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;

        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(Engine::DeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = Engine::DeviceExtensions.data();

    if (Engine::EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(Engine::ValidationLayers.size());
        createInfo.ppEnabledLayerNames = Engine::ValidationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    VK_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice),
              "Failed to create logical device")

    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
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

Device *Device::PickPhysicalDevice(const Instance *instance,
                                   const Surface *surface,
                                   const bool initLogicalDevice)
{
    static std::map<int, Device *, std::greater<>> devices;

    if (devices.empty()) {
        std::cout << "\nFetching all physical devices!\n";

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance->instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> allPhysicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance->instance, &deviceCount, allPhysicalDevices.data());

        for (const auto &physicalDevice : allPhysicalDevices) {
            Device *device = new Device(physicalDevice);
            if (device->IsDeviceSuitable(surface)) {
                LOG(device->DeviceInfoToString());
                devices.insert(std::make_pair(device->RateDeviceSuitability(), device));
            }
        }
    }

    ASSERT(devices.empty(), "Failed to find suitable GPU!");

    if (initLogicalDevice && devices.begin()->second->logicalDevice == VK_NULL_HANDLE) {
        devices.begin()->second->InitLogicalDevice();
    }

    for (auto deviceIter = ++devices.begin(); deviceIter != devices.end(); ++deviceIter)
    {
        delete deviceIter->second;
    }

    m_instance = devices.begin()->second;
    return devices.begin()->second;
}

const QueueFamilyIndices *Device::QueryQueueFamilyIndices(const Surface *surface)
{
    if (queueFamilyIndices.isComplete())
        return &queueFamilyIndices;

    QueueFamilyIndices indices;
    std::optional<uint32_t> graphicsFamily;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            break;
        }

        i++;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface->surface, &presentSupport);
    if (presentSupport) {
        indices.presentFamily = i;
    }

    queueFamilyIndices = indices;
    return &queueFamilyIndices;
}

bool Device::IsDeviceSuitable(const Surface *surface)
{
    /* Populate the queue family indices. */
    bool deviceTypeSuitable = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    if (!deviceTypeSuitable)
        return false;

    bool deviceFeaturesSuitable = deviceFeatures.geometryShader;
    if (!deviceFeaturesSuitable)
        return false;

    QueryQueueFamilyIndices(surface);
    if (!queueFamilyIndices.isComplete())
        return false;

    if (!CheckExtensionSupport())
        return false;

    QuerySwapChainSupport(surface);
    bool swapchainAdequate = !swapchainSupportDetails.formats.empty() &&
                             !swapchainSupportDetails.presentModes.empty();
    if (!swapchainAdequate)
        return false;

    return true;
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
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(Engine::DeviceExtensions.begin(),
                                             Engine::DeviceExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void Device::QuerySwapChainSupport(const Surface *surface)
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice, surface->surface, &swapchainSupportDetails.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface->surface, &formatCount, nullptr);

    if (formatCount != 0) {
        swapchainSupportDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                             surface->surface,
                                             &formatCount,
                                             swapchainSupportDetails.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice, surface->surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        swapchainSupportDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                                  surface->surface,
                                                  &presentModeCount,
                                                  swapchainSupportDetails.presentModes.data());
    }
}

uint32_t Device::FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t MemoryType = 0; MemoryType < memProperties.memoryTypeCount; MemoryType++) {
        if (TypeFilter & (1 << MemoryType) &&
            (memProperties.memoryTypes[MemoryType].propertyFlags & Properties) == Properties) {
            return MemoryType;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}
