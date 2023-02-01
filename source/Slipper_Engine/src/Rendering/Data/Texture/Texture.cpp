#include "Texture.h"

#include "Buffer.h"
#include "Drawing/CommandPool.h"
#include "GraphicsEngine.h"
#include "Path.h"

Texture::Texture(const VkImageType Type,
                 const VkExtent3D Extent,
                 const VkFormat Format,
                 const VkImageTiling Tiling,
                 const VkImageUsageFlags Usage,
                 const VkImageAspectFlags ImageAspect,
                 uint32_t ArrayLayers)
    : sampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT),
      type(Type),
      extent(Extent),
      format(Format),
      tiling(Tiling),
      usage(Usage),
      imageAspect(ImageAspect),
      arrayLayerCount(ArrayLayers),
      imageInfo(std::make_unique<VkDescriptorImageInfo>())
{
    Create();
}

void Texture::Create()
{
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = type;
    image_info.extent = extent;
    image_info.mipLevels = 1;
    image_info.arrayLayers = arrayLayerCount;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0;  // Optional

    imageInfo->imageLayout = image_info.initialLayout;

    VK_ASSERT(vkCreateImage(device, &image_info, nullptr, &texture), "Failed to create image!")

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(device, texture, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = device.FindMemoryType(mem_requirements.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_ASSERT(vkAllocateMemory(device, &alloc_info, nullptr, &textureMemory),
              "Failed to allocate image memory!")

    vkBindImageMemory(device, texture, textureMemory, 0);

    textureView = CreateImageView(texture, type, format, imageAspect, arrayLayerCount);
    imageInfo->imageView = textureView;
    imageInfo->sampler = sampler;
}

Texture::~Texture()
{
    vkDestroyImageView(device, textureView, nullptr);
    vkDestroyImage(device, texture, nullptr);
    vkFreeMemory(device, textureMemory, nullptr);
}

void Texture::Resize(const VkExtent3D Extent)
{
    extent = Extent;
    vkDestroyImageView(device, textureView, nullptr);
    vkDestroyImage(device, texture, nullptr);
    vkFreeMemory(device, textureMemory, nullptr);

    Create();
}

// TODO Format will be used for depth buffer, do not remove
SingleUseCommandBuffer Texture::CreateTransitionImageLayout(VkFormat Format,
                                                      const VkImageLayout NewLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = imageInfo->imageLayout;
    barrier.newLayout = NewLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texture;
    barrier.subresourceRange.aspectMask = imageAspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = arrayLayerCount;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    CommandPool *command_pool;
    if (barrier.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (HasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        command_pool = GraphicsEngine::Get().memoryCommandPool;
    }
    else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             barrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        command_pool = GraphicsEngine::Get().renderCommandPool;
    }
    else if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             barrier.newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        command_pool = GraphicsEngine::Get().renderCommandPool;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    SingleUseCommandBuffer command_buffer(*command_pool);
    vkCmdPipelineBarrier(
        command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    imageInfo->imageLayout = NewLayout;
    return command_buffer;
}

void Texture::CopyBuffer(const Buffer &Buffer, const bool TransitionToShaderUse)
{
    SingleUseCommandBuffer command_buffer = CreateTransitionImageLayout(
        format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = imageAspect;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = arrayLayerCount;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = extent;

    vkCmdCopyBufferToImage(
        command_buffer, Buffer, this->texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    command_buffer.Submit();

    if (TransitionToShaderUse) {
        CreateTransitionImageLayout(format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL).Submit();
    }
}

VkDescriptorImageInfo *Texture::GetDescriptorImageInfo() const
{
    return imageInfo.get();
}

VkImageView Texture::CreateImageView(VkImage image,
                                     VkImageType Type,
                                     VkFormat Format,
                                     VkImageAspectFlags ImageAspect,
                                     uint32_t ArrayLayerCount)
{
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    if (ArrayLayerCount < 2) {
        switch (Type) {
            case VK_IMAGE_TYPE_1D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case VK_IMAGE_TYPE_2D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case VK_IMAGE_TYPE_3D:
                view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
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
    view_info.subresourceRange.levelCount = 1;
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