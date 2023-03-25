#include "OffscreenSwapChain.h"

#include "GraphicsEngine.h"
#include "Core/Application.h"
#include "Texture/Texture2D.h"
#include "Window.h"

namespace Slipper
{
OffscreenSwapChain::OffscreenSwapChain(const VkExtent2D &Extent,
                                       VkFormat RenderingFormat,
                                       uint32_t NumImages,
                                       bool WithPresentationTextures)
    : SwapChain(Extent, RenderingFormat),
      withPresentationTextures(WithPresentationTextures),
      numImages(NumImages)
{
    OffscreenSwapChain::Create();
}

OffscreenSwapChain::~OffscreenSwapChain()
{
    OffscreenSwapChain::ClearImages();
}

void OffscreenSwapChain::ClearImages()
{
    SwapChain::ClearImages();

    presentationTextures.clear();

    for (const auto vk_image : GetVkImages()) {
        vkDestroyImage(device, vk_image, nullptr);
    }
    GetVkImages().clear();
    for (const auto vk_image_memory : vkImageMemory) {
        vkFreeMemory(device, vk_image_memory, nullptr);
    }
    vkImageMemory.clear();
}

void OffscreenSwapChain::UpdatePresentationTextures(VkCommandBuffer CommandBuffer,
                                                    uint32_t ImageIndex) const
{
    if (!withPresentationTextures)
        return;

    const auto viewport_resolution = GetResolution();
    presentationTextures[GetCurrentSwapChainImageIndex()]->EnqueueCopyImage(
        CommandBuffer,
        GetCurrentSwapChainImage(),
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        {viewport_resolution.width, viewport_resolution.height, 1},
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

uint32_t OffscreenSwapChain::GetCurrentSwapChainImageIndex() const
{
    return GraphicsEngine::Get().GetCurrentFrame();
}

void OffscreenSwapChain::Create(VkSwapchainKHR OldSwapChain)
{
    std::unordered_set unique_queue_families = {device.queueFamilyIndices.graphicsFamily.value(),
                                                device.queueFamilyIndices.transferFamily.value()};
    std::vector queue_families(unique_queue_families.begin(), unique_queue_families.end());

    GetVkImages().resize(numImages);
    vkImageMemory.resize(numImages);

    VkImageCreateInfo image_create_info{};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent = {resolution.width, resolution.height, 1};
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = imageRenderingFormat;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (queue_families.size() > 1) {
        image_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        image_create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size());
        image_create_info.pQueueFamilyIndices = queue_families.data();
    }
    else {
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.flags = 0;  // Optional

    for (uint32_t i = 0; i < numImages; i++) {
        VK_ASSERT(vkCreateImage(device, &image_create_info, nullptr, &GetVkImages()[i]),
                  "Failed to create image!")

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(device, GetVkImages()[i], &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = device.FindMemoryType(mem_requirements.memoryTypeBits,
                                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VK_ASSERT(vkAllocateMemory(device, &alloc_info, nullptr, &vkImageMemory[i]),
                  "Failed to allocate image memory!")

        vkBindImageMemory(device, GetVkImages()[i], vkImageMemory[i], 0);
    }

    SwapChain::Create(OldSwapChain);

    if (withPresentationTextures) {
        presentationTextures.reserve(Engine::MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < numImages; ++i) {
            presentationTextures.push_back(new Texture2D(Application::Get().window->GetSize(),
                                                         Engine::TARGET_VIEWPORT_COLOR_FORMAT,
                                                         swapChainFormat,
                                                         false));
        }
    }
}
}  // namespace Slipper