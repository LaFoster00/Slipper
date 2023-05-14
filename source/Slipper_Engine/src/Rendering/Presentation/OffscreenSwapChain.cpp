#include "OffscreenSwapChain.h"

#include "Core/Application.h"
#include "GraphicsEngine.h"
#include "Texture/Texture2D.h"
#include "Window.h"

namespace Slipper
{
OffscreenSwapChain::OffscreenSwapChain(const VkExtent2D &Extent,
                                       vk::Format RenderingFormat,
                                       uint32_t NumImages,
                                       bool WithPresentationTextures)
    : SwapChain(Extent, RenderingFormat),
      withPresentationTextures(WithPresentationTextures),
      numImages(NumImages)
{
    Create();
}

OffscreenSwapChain::~OffscreenSwapChain()
{
    OffscreenSwapChain::Impl_Cleanup(false);
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

void OffscreenSwapChain::Impl_Create()
{
    std::unordered_set unique_queue_families = {device.queueFamilyIndices.graphicsFamily.value(),
                                                device.queueFamilyIndices.transferFamily.value()};
    std::vector queue_families(unique_queue_families.begin(), unique_queue_families.end());

    GetVkImages().resize(numImages);
    vkImageMemory.resize(numImages);

    vk::ImageCreateInfo image_create_info(
        {},
        vk::ImageType::e2D,
        imageRenderingFormat,
        vk::Extent3D(resolution, 1),
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
        queue_families.size() > 1 ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
        queue_families,
        vk::ImageLayout::eUndefined);

    for (uint32_t i = 0; i < numImages; i++) {
        VK_HPP_ASSERT(
            device.logicalDevice.createImage(&image_create_info, nullptr, &GetVkImages()[i]),
            "Failed to create image");

        const vk::MemoryRequirements mem_requirements =
            device.logicalDevice.getImageMemoryRequirements(GetVkImages()[i]);

        vk::MemoryAllocateInfo alloc_info(
            mem_requirements.size,
            device.FindMemoryType(mem_requirements.memoryTypeBits,
                                  vk::MemoryPropertyFlagBits::eDeviceLocal));

        VK_HPP_ASSERT(device.logicalDevice.allocateMemory(&alloc_info, nullptr, &vkImageMemory[i]),
                      "Failed to allocate image memory")
        device.logicalDevice.bindImageMemory(GetVkImages()[i], vkImageMemory[i], 0);
    }

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

void OffscreenSwapChain::Impl_Cleanup(bool CalledFromBaseDestructor)
{
    if (CalledFromBaseDestructor)
        return;

    presentationTextures.clear();

    for (const auto vk_image : GetVkImages()) {
        device.logicalDevice.destroyImage(vk_image);
    }

    for (const auto vk_image_memory : vkImageMemory) {
        device.logicalDevice.freeMemory(vk_image_memory);
    }
}

VkSwapchainKHR OffscreenSwapChain::Impl_GetSwapChain() const
{
    return VK_NULL_HANDLE;
}
}  // namespace Slipper