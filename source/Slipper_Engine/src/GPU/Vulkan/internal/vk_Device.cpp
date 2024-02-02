#include "../vk_Device.h"

#include "Vulkan/vk_Instance.h"
#include "Vulkan/vk_Settings.h"
#include "Vulkan/vk_Surface.h"


namespace Slipper::GPU::Vulkan
{
    VKDevice *VKDevice::m_instance = nullptr;

    VKDevice::VKDevice(const vk::PhysicalDevice PhysicalDevice) : physicalDevice(PhysicalDevice)
    {
        deviceProperties = physicalDevice.getProperties();
        deviceFeatures = physicalDevice.getFeatures();
    }

    VKDevice::~VKDevice()
    {
        logicalDevice.destroy();
    }

    // Gets the next unique queue if QueueIndex is left free otherwise returns index
    vk::Queue GetQueue(uint32_t QueueFamilyIndex, std::optional<uint32_t> QueueIndex = {})
    {
        static std::unordered_map<uint32_t, uint32_t> next_queue_index;

        if (QueueIndex.has_value())
        {
            return VKDevice::Get().logicalDevice.getQueue(QueueFamilyIndex, QueueIndex.value());
        }

        return VKDevice::Get().logicalDevice.getQueue(QueueFamilyIndex, next_queue_index[QueueFamilyIndex]++);
    }

    void VKDevice::InitLogicalDevice()
    {
        // All features are checked for support during device selection
        vk::PhysicalDeviceFeatures device_features;
        device_features.setSamplerAnisotropy(VK_TRUE);

        vk::PhysicalDeviceSynchronization2Features synchronization2_features;
        synchronization2_features.setSynchronization2(VK_TRUE);

        std::vector<const char *> enabled_layers;
        if (EnableValidationLayers)
            enabled_layers = VALIDATION_LAYERS;

        auto extensions = GetRequiredExtensions();
        std::cout << "\nRequested Device Extensions:\n";
        for (const char *extension_name : extensions)
        {
            std::cout << '\t' << extension_name << '\n';
        }
        std::cout << '\n';

        {
            std::vector<vk::ExtensionProperties> available_extensions =
                physicalDevice.enumerateDeviceExtensionProperties();
            std::cout << "Available Device Extensions:\n";
            for (auto &[extension_name, spec_version] : available_extensions)
            {
                std::cout << '\t' << extension_name << '\n';
            }

            std::cout << '\n\n';
        }

        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
        std::map<uint32_t, std::vector<float>> unique_queue_family_priorities;

        unique_queue_family_priorities[queueFamilyIndices.graphicsFamily.value()].push_back(1.0f);
        unique_queue_family_priorities[queueFamilyIndices.presentFamily.value()].push_back(1.0f);
        unique_queue_family_priorities[queueFamilyIndices.transferFamily.value()].push_back(1.0f);
        unique_queue_family_priorities[queueFamilyIndices.computeFamily.value()].push_back(1.0f);

        for (auto &[queue_family, queue_priorities] : unique_queue_family_priorities)
        {

            vk::DeviceQueueCreateInfo queue_create_info({}, queue_family, queue_priorities);
            queue_create_infos.push_back(queue_create_info);
        }

        vk::DeviceCreateInfo create_info(
            {}, queue_create_infos, enabled_layers, extensions, &device_features, &synchronization2_features);

        VK_HPP_ASSERT(physicalDevice.createDevice(&create_info, nullptr, &logicalDevice),
                      "Failed to create logical device")

        computeQueue = GetQueue(queueFamilyIndices.computeFamily.value());
        graphicsQueue = GetQueue(queueFamilyIndices.graphicsFamily.value());
        presentQueue = GetQueue(queueFamilyIndices.presentFamily.value());
        transferQueue = GetQueue(queueFamilyIndices.transferFamily.value());
    }

    VKDevice *VKDevice::PickPhysicalDevice(const Surface *Surface, const bool InitLogicalDevice)
    {
        std::map<int, VKDevice *, std::greater<>> devices;

        std::cout << "\nFetching all physical devices!\n";

        for (const std::vector<vk::PhysicalDevice> physical_devices =
                 VulkanInstance::GetVk().enumeratePhysicalDevices();
             const auto &physical_device : physical_devices)
        {
            if (auto device = new VKDevice(physical_device); device->IsDeviceSuitable(Surface))
            {
                LOG(device->DeviceInfoToString());
                devices.insert(std::make_pair(device->RateDeviceSuitability(), device));
            }
        }

        ASSERT(!devices.empty(), "Failed to find suitable GPU!");

        // Save picked instance
        m_instance = devices.begin()->second;

        if (InitLogicalDevice && !devices.empty())
        {
            devices.begin()->second->InitLogicalDevice();
        }

        // Delete the rest of the instances
        devices.erase(devices.begin()->first);

        for (const auto new_device : devices | std::ranges::views::values)
        {
            delete new_device;
        }

        return m_instance;
    }

    std::string VKDevice::DeviceInfoToString() const
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

        info += "\n";

        return info;
    }

    std::vector<const char *> VKDevice::GetRequiredExtensions()
    {
        std::unordered_set<const char *> extensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        extensions.insert(glfw_extensions, glfw_extensions + glfw_extension_count);

        // This removes all the instance extensions that might have found their way into this (looking
        // at you glfw)
        for (auto extension = extensions.begin(); extension != extensions.end();)
        {
            bool same = false;
            for (const auto &[extension_name, spec] : vk::enumerateInstanceExtensionProperties())
            {
                if (strcmp(*extension, extension_name) == 0)
                {
                    extension = extensions.erase(extension);
                    same = true;
                    break;
                }
            }
            if (!same)
                ++extension;
        }

        return std::vector(extensions.begin(), extensions.end());
    }

    vk::SurfaceCapabilitiesKHR VKDevice::GetPhysicalDeviceSurfaceCapabilities(const Surface *Surface) const
    {
        return physicalDevice.getSurfaceCapabilitiesKHR(Surface->vkSurface);
    }

    uint32_t VKDevice::SurfaceSwapChainImagesCount(const Surface *Surface) const
    {
        const auto swap_chain_support = QuerySwapChainSupport(Surface);
        return CapabilitiesSwapChainImageCount(swap_chain_support);
    }

    uint32_t VKDevice::CapabilitiesSwapChainImageCount(const SwapChainSupportDetails &Support)
    {
        return std::clamp(
            static_cast<uint32_t>(Support.capabilities.minImageCount),
            static_cast<uint32_t>(0),
            static_cast<uint32_t>(std::max(Support.capabilities.maxImageCount, Support.capabilities.minImageCount)));
    }

    SwapChainSupportDetails VKDevice::QuerySwapChainSupport(const Surface *Surface) const
    {
        SwapChainSupportDetails swap_chain_support_details;
        swap_chain_support_details.capabilities = GetPhysicalDeviceSurfaceCapabilities(Surface);
        swap_chain_support_details.formats = physicalDevice.getSurfaceFormatsKHR(Surface->vkSurface);
        swap_chain_support_details.presentModes = physicalDevice.getSurfacePresentModesKHR(Surface->vkSurface);
        return swap_chain_support_details;
    }

    VkExtent2D VKDevice::GetSurfaceResolution(const Surface *Surface) const
    {
        return GetPhysicalDeviceSurfaceCapabilities(Surface).currentExtent;
    }

    vk::SampleCountFlagBits VKDevice::GetMaxUsableFramebufferSampleCount() const
    {
        const vk::SampleCountFlags counts = deviceProperties.limits.framebufferColorSampleCounts &
            deviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & vk::SampleCountFlagBits::e64)
        {
            return vk::SampleCountFlagBits::e64;
        }
        if (counts & vk::SampleCountFlagBits::e32)
        {
            return vk::SampleCountFlagBits::e32;
        }
        if (counts & vk::SampleCountFlagBits::e16)
        {
            return vk::SampleCountFlagBits::e16;
        }
        if (counts & vk::SampleCountFlagBits::e8)
        {
            return vk::SampleCountFlagBits::e8;
        }
        if (counts & vk::SampleCountFlagBits::e4)
        {
            return vk::SampleCountFlagBits::e4;
        }
        if (counts & vk::SampleCountFlagBits::e2)
        {
            return vk::SampleCountFlagBits::e2;
        }
        return vk::SampleCountFlagBits::e1;
    }

    uint32_t VKDevice::FindMemoryType(const uint32_t TypeFilter, const vk::MemoryPropertyFlags Properties) const
    {
        const vk::PhysicalDeviceMemoryProperties mem_properties = physicalDevice.getMemoryProperties();

        for (uint32_t memory_type = 0; memory_type < mem_properties.memoryTypeCount; memory_type++)
        {
            if (TypeFilter & (1 << memory_type) &&
                (mem_properties.memoryTypes[memory_type].propertyFlags & Properties) == Properties)
            {
                return memory_type;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }

    bool VKDevice::IsDeviceSuitable(const Surface *Surface)
    {
        const auto [capabilities, formats, presentModes] = QuerySwapChainSupport(Surface);
        /* Populate the queue family indices. */
        if (const bool device_type_suitable = deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu;
            !device_type_suitable)
            return false;

        QueryQueueFamilyIndices(Surface);
        if (!queueFamilyIndices.IsComplete())
            return false;

        if (!CheckExtensionSupport() || !CheckFeatureSupport())
            return false;

        return !formats.empty() && !presentModes.empty();
    }

    uint32_t VKDevice::RateDeviceSuitability() const
    {
        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader)
        {
            return 0;
        }

        return score;
    }

    bool VKDevice::CheckExtensionSupport() const
    {
        std::vector<vk::ExtensionProperties> available_extensions = physicalDevice.enumerateDeviceExtensionProperties();

        std::set<std::string> extensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());
        auto required_extensions = GetRequiredExtensions();
        extensions.insert(required_extensions.begin(), required_extensions.end());

        for (const auto &[extension_name, spec_version] : available_extensions)
        {
            extensions.erase(extension_name);
        }

        return extensions.empty();
    }

    bool VKDevice::CheckFeatureSupport() const
    {
        const vk::PhysicalDeviceFeatures supported_features = physicalDevice.getFeatures();

        return supported_features.samplerAnisotropy && supported_features.geometryShader;
    }

    uint32_t SelectQueue(const std::vector<vk::QueueFamilyProperties> &QueueFamilies,
                         const std::unordered_set<uint32_t> &TakenFamilies,
                         const vk::QueueFlags QueueFlags)
    {
        // All the viable candidates for the currently searched queue
        std::vector<size_t> candidates;
        // The candidates sorted by how many different kinds of flags they have
        std::map<uint32_t, size_t> smallest_candidate;

        for (size_t family_index = 0; family_index < QueueFamilies.size(); ++family_index)
        {
            if ((QueueFamilies[family_index].queueFlags & QueueFlags) == QueueFlags)
            {
                candidates.push_back(family_index);
                uint32_t supported_operations = 0;
                if (QueueFamilies[family_index].queueFlags & vk::QueueFlagBits::eCompute)
                    ++supported_operations;
                if (QueueFamilies[family_index].queueFlags & vk::QueueFlagBits::eGraphics)
                    ++supported_operations;
                if (QueueFamilies[family_index].queueFlags & vk::QueueFlagBits::eOpticalFlowNV)
                    ++supported_operations;
                if (QueueFamilies[family_index].queueFlags & vk::QueueFlagBits::eProtected)
                    ++supported_operations;
                if (QueueFamilies[family_index].queueFlags & vk::QueueFlagBits::eSparseBinding)
                    ++supported_operations;
                if (QueueFamilies[family_index].queueFlags & vk::QueueFlagBits::eTransfer)
                    ++supported_operations;
                if (QueueFamilies[family_index].queueFlags & vk::QueueFlagBits::eVideoDecodeKHR)
                    ++supported_operations;
                smallest_candidate.insert({supported_operations, family_index});
            }
        }

        for (const auto queue_family : smallest_candidate | std::views::values)
        {
            if (!TakenFamilies.contains(queue_family))
            {
                return queue_family;
            }
        }

        if (!smallest_candidate.empty())
        {
            return smallest_candidate.begin()->second;
        }

        throw std::invalid_argument("No queue family with requested queue flags exists");
    }

    const QueueFamilyIndices *VKDevice::QueryQueueFamilyIndices(const Surface *Surface)
    {
        if (queueFamilyIndices.IsComplete())
            return &queueFamilyIndices;

        QueueFamilyIndices indices;

        // All the different queue families
        const std::vector<vk::QueueFamilyProperties> queue_families = physicalDevice.getQueueFamilyProperties();

        for (size_t i = 0; i < queue_families.size(); ++i)
        {
            LOG_FORMAT("Queue Family [{}] Count:{} upports:", i, queue_families[i].queueCount)
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eCompute)
                LOG("\tCompute");
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics)
                LOG("\tGraphics");
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eOpticalFlowNV)
                LOG("\tOpticalFlowNV");
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eProtected)
                LOG("\tProtected");
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eSparseBinding)
                LOG("\tSparseBinding");
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eTransfer)
                LOG("\tTransfer");
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eVideoDecodeKHR)
                LOG("\tVideoDecodeKHR");
        }

        // All the queue families that are already taken
        std::unordered_set<uint32_t> taken_families;

        // Get the queue family that supports graphics operations
        indices.graphicsFamily = SelectQueue(queue_families, taken_families, vk::QueueFlagBits::eGraphics);
        taken_families.insert(indices.graphicsFamily.value());

        indices.computeFamily = SelectQueue(queue_families, taken_families, vk::QueueFlagBits::eCompute);
        taken_families.insert(indices.computeFamily.value());

        indices.transferFamily = SelectQueue(
            queue_families, taken_families, vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eGraphics);
        taken_families.insert(indices.transferFamily.value());

        // Get the queue family with surface support
        for (uint32_t family_index = 0; family_index < static_cast<uint32_t>(queue_families.size()); ++family_index)
        {
            if (physicalDevice.getSurfaceSupportKHR(family_index, *Surface))
                indices.presentFamily = family_index;
        }

        queueFamilyIndices = indices;
        return &queueFamilyIndices;
    }
}  // namespace Slipper
