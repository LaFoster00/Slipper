#include "OffscreenSwapChain.h"

#include <unordered_set>

#include "common_defines.h"
#include "Setup/GraphicsSettings.h"

namespace Slipper
{
OffscreenSwapChain::OffscreenSwapChain(const VkExtent2D &Extent,
                                       VkFormat Format,
                                       uint32_t NumImages)
    : SwapChain(Extent, Format), numImages(NumImages)
{
    OffscreenSwapChain::Create();
}

OffscreenSwapChain::~OffscreenSwapChain()
{
    for (const auto vk_image_view : vkImageViews)
    {
        vkDestroyImageView(device, vk_image_view, nullptr);
    }
    vkImageViews.clear();
    for (const auto vk_image : vkImages)
    {
        vkDestroyImage(device, vk_image, nullptr);
    }
    vkImages.clear();
    for (const auto vk_image_memory : vkImageMemory)
    {
        vkFreeMemory(device, vk_image_memory, nullptr);
    }
    vkImageMemory.clear();
}

void OffscreenSwapChain::Create(VkSwapchainKHR OldSwapChain)
{
    std::unordered_set unique_queue_families = {device.queueFamilyIndices.graphicsFamily.value(),
                                                device.queueFamilyIndices.transferFamily.value()};
    std::vector queue_families(unique_queue_families.begin(), unique_queue_families.end());

    vkImages.resize(numImages);
    vkImageMemory.resize(numImages);

    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent = {resolution.width, resolution.height, 1};
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = imageFormat;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (queue_families.size() > 1) {
        image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        image_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size());
        image_info.pQueueFamilyIndices = queue_families.data();
    }
    else {
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0;  // Optional

    for (uint32_t i = 0; i < numImages; i++)
    {
        VK_ASSERT(vkCreateImage(device, &image_info, nullptr, &vkImages[i]), "Failed to create image!")

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(device, vkImages[i], &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = device.FindMemoryType(mem_requirements.memoryTypeBits,
                                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VK_ASSERT(vkAllocateMemory(device, &alloc_info, nullptr, &vkImageMemory[i]),
                  "Failed to allocate image memory!")

        vkBindImageMemory(device, vkImages[i], vkImageMemory[i], 0);
    }
    SwapChain::Create(OldSwapChain);
}
}  // namespace Slipper