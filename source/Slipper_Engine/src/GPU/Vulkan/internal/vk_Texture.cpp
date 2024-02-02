#include "../vk_Texture.h"

#include "Vulkan/vk_Device.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Slipper::GPU::Vulkan
{
Texture::Texture(const vk::ImageType Type,
                 const vk::Extent3D Extent,
                 const vk::Format ImageFormat,
                 std::optional<vk::Format> ViewFormat,
                 const bool GenerateMipMaps,
                 const vk::SampleCountFlagBits NumSamples,
                 const vk::ImageTiling Tiling,
                 const vk::ImageUsageFlags Usage,
                 const vk::ImageAspectFlags ImageAspect,
                 const uint32_t ArrayLayers)
    : imageInfo{VK_NULL_HANDLE,
                {},
                vk::ImageLayout::eUndefined,
                Type,
                Extent,
                ViewFormat.has_value() ? ViewFormat.value() : ImageFormat,
                ImageFormat,
                Tiling,
                Usage,
                NumSamples,
                ImageAspect,
                ArrayLayers,
                GenerateMipMaps,
                0}
{
    if (imageInfo.generateMipMaps) {
        imageInfo.usage |= vk::ImageUsageFlagBits::eTransferSrc |
                           vk::ImageUsageFlagBits::eTransferDst;
        imageInfo.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(
                                  std::max(imageInfo.extent.width, imageInfo.extent.height),
                                  imageInfo.extent.depth)))) +
                              1;
    }
    else {
        imageInfo.mipLevels = 1;
    }

    sampler = Sampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageInfo.mipLevels);

    Create();
}

void Texture::Create()
{
    std::unordered_set unique_queue_families = {device.queueFamilyIndices.graphicsFamily.value(),
                                                device.queueFamilyIndices.transferFamily.value()};
    std::vector queue_families(unique_queue_families.begin(), unique_queue_families.end());

    const vk::ImageCreateInfo image_create_info(
        vk::ImageCreateFlagBits::eMutableFormat,
        imageInfo.type,
        imageInfo.imageFormat,
        imageInfo.extent,
        imageInfo.mipLevels,
        imageInfo.arrayLayerCount,
        imageInfo.numSamples,
        imageInfo.tiling,
        imageInfo.usage,
        queue_families.size() > 1 ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
        queue_families,
        imageInfo.layout);

    VK_HPP_ASSERT(device.logicalDevice.createImage(&image_create_info, nullptr, &vkImage),
                  "Failed to create image!")

    const VkMemoryRequirements mem_requirements = device.logicalDevice.getImageMemoryRequirements(
        vkImage);
    vk::MemoryAllocateInfo alloc_info(
        mem_requirements.size,
        device.FindMemoryType(mem_requirements.memoryTypeBits,
                              vk::MemoryPropertyFlagBits::eDeviceLocal));

    VK_HPP_ASSERT(device.logicalDevice.allocateMemory(&alloc_info, nullptr, &vkImageMemory),
                  "Failed to allocate image memory!");
    device.logicalDevice.bindImageMemory(vkImage, vkImageMemory, 0);

    imageInfo.views.push_back(CreateImageView(vkImage,
                                              imageInfo.type,
                                              imageInfo.viewFormat,
                                              imageInfo.mipLevels,
                                              imageInfo.imageAspect,
                                              imageInfo.arrayLayerCount));
    imageInfo.sampler = sampler;
}

void Texture::EnqueueGenerateMipMaps(vk::CommandBuffer CommandBuffer)
{
    // Check if image format supports linear blitting
    vk::FormatProperties formatProperties = device.physicalDevice.getFormatProperties(
        imageInfo.imageFormat);

    if (!(formatProperties.optimalTilingFeatures &
          vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    vk::ImageMemoryBarrier barrier;
    barrier.image = vkImage;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mip_width = imageInfo.extent.width;
    int32_t mip_height = imageInfo.extent.height;

    for (uint32_t i = 1; i < imageInfo.mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                      vk::PipelineStageFlagBits::eTransfer,
                                      vk::DependencyFlags{},
                                      nullptr,
                                      nullptr,
                                      barrier);

        vk::ImageBlit blit(
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1, 0, 1),
            {vk::Offset3D(0, 0, 0), vk::Offset3D(mip_width, mip_height, 1)},
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, 0, 1),
            {vk::Offset3D(0, 0, 0),
             vk::Offset3D(
                 mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1)});

        blit.setSrcSubresource(
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1, 0, 1));
        blit.setSrcOffsets({vk::Offset3D(0, 0, 0), vk::Offset3D(mip_width, mip_height, 1)});

        blit.setDstSubresource(
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, 0, 1));
        blit.setDstOffsets({vk::Offset3D(0, 0, 0),
                            vk::Offset3D(mip_width > 1 ? mip_width / 2 : 1,
                                         mip_height > 1 ? mip_height / 2 : 1,
                                         1)});
        CommandBuffer.blitImage(vkImage,
                                vk::ImageLayout::eTransferSrcOptimal,
                                vkImage,
                                vk::ImageLayout::eTransferDstOptimal,
                                blit,
                                vk::Filter::eLinear);

        barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
        barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                      vk::PipelineStageFlagBits::eFragmentShader,
                                      vk::DependencyFlags{},
                                      nullptr,
                                      nullptr,
                                      barrier);

        if (mip_width > 1)
            mip_width /= 2;
        if (mip_height > 1)
            mip_height /= 2;
    }

    barrier.subresourceRange.baseMipLevel = imageInfo.mipLevels - 1;
    barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
    barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
    barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

    CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                  vk::PipelineStageFlagBits::eFragmentShader,
                                  vk::DependencyFlags{},
                                  nullptr,
                                  nullptr,
                                  barrier);

    imageInfo.layout = barrier.newLayout;
}

Texture::~Texture()
{
    for (const auto vk_image_view : imageInfo.views) {
        device.logicalDevice.destroyImageView(vk_image_view, nullptr);
    }
    imageInfo.views.clear();
    device.logicalDevice.destroyImage(vkImage, nullptr);
    device.logicalDevice.freeMemory(vkImageMemory, nullptr);
}

void Texture::Resize(const vk::Extent3D Extent)
{
    imageInfo.extent = Extent;

    for (const auto vk_image_view : imageInfo.views) {
        device.logicalDevice.destroyImageView(vk_image_view, nullptr);
    }
    imageInfo.views.clear();
    device.logicalDevice.destroyImage(vkImage, nullptr);
    device.logicalDevice.freeMemory(vkImageMemory, nullptr);

    Create();
}

void Texture::EnqueueTransitionImageLayout(vk::Image Image,
                                           ImageInfo &ImageInfo,
                                           vk::CommandBuffer CommandBuffer,
                                           vk::ImageLayout NewLayout)
{
    vk::ImageMemoryBarrier2 barrier;
    barrier.oldLayout = ImageInfo.layout;
    barrier.newLayout = NewLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = Image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = ImageInfo.mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = ImageInfo.arrayLayerCount;

    if (barrier.newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        if (HasStencilComponent(ImageInfo.imageFormat)) {
            barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    }
    else {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    if (barrier.oldLayout == vk::ImageLayout::eUndefined) {
        barrier.srcAccessMask = {};
        barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
    }
    else if (barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
        barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
    }
    else if (barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits2::eTransferRead;
        barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
    }
    else if (barrier.oldLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits2::eShaderRead;
        barrier.srcStageMask = vk::PipelineStageFlagBits2::eFragmentShader |
                               vk::PipelineStageFlagBits2::eVertexShader;
    }
    else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    if (barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
        barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader |
                               vk::PipelineStageFlagBits2::eVertexShader;
    }
    else if (barrier.newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
        barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
    }
    else if (barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
        barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
    }
    else if (barrier.newLayout == vk::ImageLayout::eDepthAttachmentOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentRead |
                                vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
        barrier.dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests;
    }
    else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vk::DependencyInfo dependency_info({}, nullptr, nullptr, barrier);
    CommandBuffer.pipelineBarrier2(dependency_info);

    ImageInfo.layout = NewLayout;
}

SingleUseCommandBuffer Texture::CreateTransitionImageLayout(vk::Image Image,
                                                            ImageInfo &ImageInfo,
                                                            const vk::ImageLayout NewLayout)
{
    const auto old_layout = ImageInfo.layout;
    CommandPool *command_pool;

    if (old_layout == vk::ImageLayout::eUndefined &&
        NewLayout == vk::ImageLayout::eTransferDstOptimal) {
        command_pool = GraphicsEngine::Get().memoryCommandPool.get();
    }
    else if (old_layout == vk::ImageLayout::eTransferDstOptimal &&
             NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        command_pool = GraphicsEngine::Get().GetViewportCommandPool();
    }
    else if (old_layout == vk::ImageLayout::eUndefined &&
             NewLayout == vk::ImageLayout::eDepthAttachmentOptimal) {
        command_pool = GraphicsEngine::Get().GetViewportCommandPool();
    }
    else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    SingleUseCommandBuffer command_buffer(*command_pool);
    EnqueueTransitionImageLayout(Image, ImageInfo, command_buffer, NewLayout);
    return command_buffer;
}

void Texture::CopyBuffer(const Buffer &Buffer, const bool TransitionToShaderUse)
{
    SingleUseCommandBuffer command_buffer = CreateTransitionImageLayout(
        vkImage, imageInfo, vk::ImageLayout::eTransferDstOptimal);

    vk::BufferImageCopy2 region(
        0,
        0,
        0,
        vk::ImageSubresourceLayers(imageInfo.imageAspect, 0, 0, imageInfo.arrayLayerCount),
        {0, 0, 0},
        imageInfo.extent);

    const vk::CopyBufferToImageInfo2 copy_buffer_to_image_info(
        Buffer, vkImage, vk::ImageLayout::eTransferDstOptimal, region);

    command_buffer.Get().copyBufferToImage2(copy_buffer_to_image_info);

    if (!imageInfo.generateMipMaps) {
        if (TransitionToShaderUse) {
            EnqueueTransitionImageLayout(
                vkImage, imageInfo, command_buffer, vk::ImageLayout::eShaderReadOnlyOptimal);
        }
    }
    else {
        EnqueueGenerateMipMaps(command_buffer);
    }

    command_buffer.Submit();
}

void Texture::EnqueueCopyImage(vk::CommandBuffer CommandBuffer,
                               vk::Image SrcImage,
                               vk::ImageLayout SrcLayout,
                               vk::Extent3D SrcExtent,
                               vk::ImageLayout TargetLayout)
{
    EnqueueTransitionImageLayout(
        vkImage, imageInfo, CommandBuffer, vk::ImageLayout::eTransferDstOptimal);

    vk::ImageBlit2 blit(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
                        {vk::Offset3D(0, 0, 0),
                         vk::Offset3D(static_cast<int32_t>(SrcExtent.width),
                                      static_cast<int32_t>(SrcExtent.height),
                                      static_cast<int32_t>(SrcExtent.depth))},
                        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
                        {vk::Offset3D(0, 0, 0),
                         vk::Offset3D(static_cast<int32_t>(imageInfo.extent.width),
                                      static_cast<int32_t>(imageInfo.extent.height),
                                      static_cast<int32_t>(imageInfo.extent.depth))});
    const vk::BlitImageInfo2 blit_info(
        SrcImage, SrcLayout, vkImage, imageInfo.layout, blit, vk::Filter::eNearest);
    CommandBuffer.blitImage2(blit_info);

    EnqueueTransitionImageLayout(vkImage, imageInfo, CommandBuffer, TargetLayout);
}

void Texture::CopyImage(const vk::Image SrcImage,
                        const vk::ImageLayout SrcLayout,
                        const vk::Extent3D SrcExtent,
                        const vk::ImageLayout TargetLayout)
{
    SingleUseCommandBuffer command_buffer = SingleUseCommandBuffer(
        *GraphicsEngine::Get().memoryCommandPool);
    EnqueueCopyImage(command_buffer, SrcImage, SrcLayout, SrcExtent, TargetLayout);
    command_buffer.Submit();
}

void Texture::EnqueueCopyTexture(vk::CommandBuffer CommandBuffer,
                                 Texture &Texture,
                                 vk::ImageLayout TargetLayout)
{
    const auto previous_other_texture_layout = Texture.GetCurrentLayout();
    Texture.EnqueueTransitionImageLayout(
        vkImage, imageInfo, CommandBuffer, vk::ImageLayout::eTransferSrcOptimal);
    EnqueueCopyImage(CommandBuffer,
                     Texture.vkImage,
                     Texture.GetCurrentLayout(),
                     Texture.imageInfo.extent,
                     TargetLayout);
    Texture.EnqueueTransitionImageLayout(
        vkImage, imageInfo, CommandBuffer, previous_other_texture_layout);
}

void Texture::CopyTexture(Texture Texture, vk::ImageLayout TargetLayout)
{
    auto command_buffer = SingleUseCommandBuffer(*GraphicsEngine::Get().memoryCommandPool);
    EnqueueCopyTexture(command_buffer, Texture, TargetLayout);
    command_buffer.Submit();
}

vk::ImageView Texture::CreateImageView(vk::Image Image,
                                       vk::ImageType Type,
                                       vk::Format Format,
                                       uint32_t MipLevels,
                                       vk::ImageAspectFlags ImageAspect,
                                       uint32_t ArrayLayerCount)
{
    vk::ImageViewCreateInfo view_info(
        vk::ImageViewCreateFlags{},
        Image,
        vk::ImageViewType::e2D,  // Overwritten below
        Format,
        vk::ComponentMapping(),
        vk::ImageSubresourceRange(ImageAspect, 0, MipLevels, 0, ArrayLayerCount));

    if (ArrayLayerCount < 2) {
        switch (Type) {
            case vk::ImageType::e1D:
                view_info.viewType = vk::ImageViewType::e1D;
                break;
            case vk::ImageType::e2D:
                view_info.viewType = vk::ImageViewType::e2D;
                break;
            case vk::ImageType::e3D:
                view_info.viewType = vk::ImageViewType::e3D;
                break;
        }
    }
    else {
        switch (Type) {
            case vk::ImageType::e1D:
                view_info.viewType = vk::ImageViewType::e1DArray;
                break;
            case vk::ImageType::e2D:
                view_info.viewType = vk::ImageViewType::e2DArray;
                break;
            case vk::ImageType::e3D:
                view_info.viewType = vk::ImageViewType::eCubeArray;
                break;
        }
    }

    vk::ImageView image_view;
    VK_HPP_ASSERT(VKDevice::Get().logicalDevice.createImageView(&view_info, nullptr, &image_view),
                  "Failed to create texture image view!");
    return image_view;
}

vk::ImageView Texture::CreateDefaultImageView() const
{
    return CreateImageView(vkImage,
                           imageInfo.type,
                           imageInfo.viewFormat,
                           imageInfo.mipLevels,
                           imageInfo.imageAspect,
                           imageInfo.arrayLayerCount);
}

vk::ImageView Texture::CreateImageView(vk::Format ViewFormat) const
{
    return CreateImageView(vkImage,
                           imageInfo.type,
                           ViewFormat,
                           imageInfo.mipLevels,
                           imageInfo.imageAspect,
                           imageInfo.arrayLayerCount);
}

vk::Format Texture::FindSupportedFormat(const std::vector<vk::Format> &Candidates,
                                        const vk::ImageTiling Tiling,
                                        const vk::FormatFeatureFlags Features)
{
    for (const vk::Format format : Candidates) {
        if (auto props = VKDevice::Get().physicalDevice.getFormatProperties(format);
            Tiling == vk::ImageTiling::eLinear &&
            (props.linearTilingFeatures & Features) == Features) {
            return format;
        }
        else if (Tiling == vk::ImageTiling::eOptimal &&
                 (props.optimalTilingFeatures & Features) == Features) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

vk::Format Texture::FindDepthFormat()
{
    return FindSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

bool Texture::HasStencilComponent(const vk::Format Format)
{
    return Format == vk::Format::eD32SfloatS8Uint || Format == vk::Format::eD24UnormS8Uint;
}
}  // namespace Slipper