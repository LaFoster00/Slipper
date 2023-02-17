#include "Texture.h"

#include "Buffer.h"
#include "Drawing/CommandPool.h"
#include "GraphicsEngine.h"
#include "Path.h"
#include "Util/StringUtil.h"

namespace Slipper
{
Texture::Texture(const VkImageType Type,
                 const VkExtent3D Extent,
                 const VkFormat ImageFormat,
                 std::optional<VkFormat> ViewFormat,
                 const bool GenerateMipMaps,
                 const VkSampleCountFlagBits NumSamples,
                 const VkImageTiling Tiling,
                 const VkImageUsageFlags Usage,
                 const VkImageAspectFlags ImageAspect,
                 const uint32_t ArrayLayers)
    : imageInfo{VK_NULL_HANDLE,
                VK_NULL_HANDLE,
                VK_IMAGE_LAYOUT_UNDEFINED,
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
        imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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

    VkImageCreateInfo image_create_info{};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = imageInfo.type;
    image_create_info.extent = imageInfo.extent;
    image_create_info.mipLevels = imageInfo.mipLevels;
    image_create_info.arrayLayers = imageInfo.arrayLayerCount;
    image_create_info.format = imageInfo.imageFormat;
    image_create_info.tiling = imageInfo.tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = imageInfo.usage;
    if (queue_families.size() > 1) {
        image_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        image_create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size());
        image_create_info.pQueueFamilyIndices = queue_families.data();
    }
    else {
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    image_create_info.samples = imageInfo.numSamples;
    image_create_info.flags = 0;  // Optional
    if (imageInfo.viewFormat != imageInfo.imageFormat)
        image_create_info.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

    imageInfo.layout = image_create_info.initialLayout;

    VK_ASSERT(vkCreateImage(device, &image_create_info, nullptr, &vkImage),
              "Failed to create image!")

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(device, vkImage, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = device.FindMemoryType(mem_requirements.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_ASSERT(vkAllocateMemory(device, &alloc_info, nullptr, &vkImageMemory),
              "Failed to allocate image memory!")

    vkBindImageMemory(device, vkImage, vkImageMemory, 0);

    imageInfo.view = CreateImageView(vkImage,
                                     imageInfo.type,
                                     imageInfo.viewFormat,
                                     imageInfo.mipLevels,
                                     imageInfo.imageAspect,
                                     imageInfo.arrayLayerCount);
    imageInfo.sampler = sampler;
}

void Texture::EnqueueGenerateMipMaps(VkCommandBuffer CommandBuffer)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(device, imageInfo.imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures &
          VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = vkImage;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mip_width = imageInfo.extent.width;
    int32_t mip_height = imageInfo.extent.height;

    for (uint32_t i = 1; i < imageInfo.mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(CommandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mip_width, mip_height, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {
            mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(CommandBuffer,
                       vkImage,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       vkImage,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(CommandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        if (mip_width > 1)
            mip_width /= 2;
        if (mip_height > 1)
            mip_height /= 2;
    }

    barrier.subresourceRange.baseMipLevel = imageInfo.mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(CommandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    imageInfo.layout = barrier.newLayout;
}

Texture::~Texture()
{
    vkDestroyImageView(device, imageInfo.view, nullptr);
    vkDestroyImage(device, vkImage, nullptr);
    vkFreeMemory(device, vkImageMemory, nullptr);
}

void Texture::Resize(const VkExtent3D Extent)
{
    imageInfo.extent = Extent;
    vkDestroyImageView(device, imageInfo.view, nullptr);
    vkDestroyImage(device, vkImage, nullptr);
    vkFreeMemory(device, vkImageMemory, nullptr);

    Create();
}

void Texture::EnqueueTransitionImageLayout(VkImage Image,
                                           ImageInfo &ImageInfo,
                                           VkCommandBuffer CommandBuffer,
                                           VkImageLayout NewLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = ImageInfo.layout;
    barrier.newLayout = NewLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = Image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = ImageInfo.mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = ImageInfo.arrayLayerCount;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (barrier.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (HasStencilComponent(ImageInfo.imageFormat)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
        barrier.srcAccessMask = 0;
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
    else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (barrier.oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        source_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    if (barrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    }
    else if (barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (barrier.newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        CommandBuffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    ImageInfo.layout = NewLayout;
}

SingleUseCommandBuffer Texture::CreateTransitionImageLayout(VkImage Image,
                                                            ImageInfo &ImageInfo,
                                                            const VkImageLayout NewLayout)
{
    const auto old_layout = ImageInfo.layout;
    CommandPool *command_pool;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        command_pool = GraphicsEngine::Get().memoryCommandPool.get();
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        command_pool = GraphicsEngine::Get().drawCommandPool.get();
    }
    else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
             NewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {
        command_pool = GraphicsEngine::Get().drawCommandPool.get();
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
        vkImage, imageInfo, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = imageInfo.imageAspect;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = imageInfo.arrayLayerCount;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = imageInfo.extent;

    vkCmdCopyBufferToImage(
        command_buffer, Buffer, this->vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    if (!imageInfo.generateMipMaps) {
        if (TransitionToShaderUse) {
            EnqueueTransitionImageLayout(
                vkImage, imageInfo, command_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }
    else {
        EnqueueGenerateMipMaps(command_buffer);
    }

    command_buffer.Submit();
}

void Texture::EnqueueCopyImage(VkCommandBuffer CommandBuffer,
                               VkImage SrcImage,
                               VkImageLayout SrcLayout,
                               VkExtent3D SrcExtent,
                               VkImageLayout TargetLayout)
{
    EnqueueTransitionImageLayout(
        vkImage, imageInfo, CommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {static_cast<int32_t>(SrcExtent.width),
                          static_cast<int32_t>(SrcExtent.height),
                          static_cast<int32_t>(SrcExtent.depth)};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = 0;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {static_cast<int32_t>(imageInfo.extent.width),
                          static_cast<int32_t>(imageInfo.extent.height),
                          static_cast<int32_t>(imageInfo.extent.depth)};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = 0;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(CommandBuffer,
                   SrcImage,
                   SrcLayout,
                   vkImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &blit,
                   VK_FILTER_LINEAR);

    EnqueueTransitionImageLayout(vkImage, imageInfo, CommandBuffer, TargetLayout);
}

void Texture::CopyImage(const VkImage SrcImage,
                        const VkImageLayout SrcLayout,
                        const VkExtent3D SrcExtent,
                        const VkImageLayout TargetLayout)
{
    SingleUseCommandBuffer command_buffer = SingleUseCommandBuffer(
        *GraphicsEngine::Get().memoryCommandPool);
    EnqueueCopyImage(command_buffer, SrcImage, SrcLayout, SrcExtent, TargetLayout);
    command_buffer.Submit();
}

void Texture::EnqueueCopyTexture(VkCommandBuffer CommandBuffer,
                                 Texture &Texture,
                                 VkImageLayout TargetLayout)
{
    const auto previous_other_texture_layout = Texture.GetCurrentLayout();
    Texture.EnqueueTransitionImageLayout(
        vkImage, imageInfo, CommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    EnqueueCopyImage(CommandBuffer,
                     Texture.vkImage,
                     Texture.GetCurrentLayout(),
                     Texture.imageInfo.extent,
                     TargetLayout);
    Texture.EnqueueTransitionImageLayout(
        vkImage, imageInfo, CommandBuffer, previous_other_texture_layout);
}

void Texture::CopyTexture(Texture Texture, VkImageLayout TargetLayout)
{
    SingleUseCommandBuffer command_buffer = SingleUseCommandBuffer(
        *GraphicsEngine::Get().memoryCommandPool);
    EnqueueCopyTexture(command_buffer, Texture, TargetLayout);
    command_buffer.Submit();
}

VkImageView Texture::CreateImageView(VkImage Image,
                                     VkImageType Type,
                                     VkFormat Format,
                                     uint32_t MipLevels,
                                     VkImageAspectFlags ImageAspect,
                                     uint32_t ArrayLayerCount)
{
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = Image;
    if (ArrayLayerCount < 2) {
        switch (Type) {
            case VK_IMAGE_TYPE_1D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case VK_IMAGE_TYPE_2D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case VK_IMAGE_TYPE_3D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_3D;
                break;
        }
    }
    else {
        switch (Type) {
            case VK_IMAGE_TYPE_1D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                break;
            case VK_IMAGE_TYPE_2D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                break;
            case VK_IMAGE_TYPE_3D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                break;
        }
    }
    view_info.format = Format;
    view_info.subresourceRange.aspectMask = ImageAspect;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = MipLevels;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = ArrayLayerCount;

    VkImageView image_view;
    VK_ASSERT(vkCreateImageView(Device::Get(), &view_info, nullptr, &image_view),
              "Failed to create texture image view!");
    return image_view;
}

VkFormat Texture::FindSupportedFormat(const std::vector<VkFormat> &Candidates,
                                      const VkImageTiling Tiling,
                                      const VkFormatFeatureFlags Features)
{
    for (const VkFormat format : Candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(Device::Get().physicalDevice, format, &props);

        if (Tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & Features) == Features) {
            return format;
        }
        else if (Tiling == VK_IMAGE_TILING_OPTIMAL &&
                 (props.optimalTilingFeatures & Features) == Features) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

VkFormat Texture::FindDepthFormat()
{
    return FindSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool Texture::HasStencilComponent(const VkFormat Format)
{
    return Format == VK_FORMAT_D32_SFLOAT_S8_UINT || Format == VK_FORMAT_D24_UNORM_S8_UINT;
}
}  // namespace Slipper