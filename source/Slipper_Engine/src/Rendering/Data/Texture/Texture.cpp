#include "Texture.h"

#include "Buffer.h"
#include "Drawing/CommandPool.h"
#include "Path.h"

Texture::Texture(const VkImageType Type, const VkExtent3D Extent, const VkFormat Format)
    : type(Type), extent(Extent), format(Format)
{
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = type;
    image_info.extent = extent;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0;  // Optional

    layout = image_info.initialLayout;

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
}

Texture::~Texture()
{
    vkDestroyImage(device, texture, nullptr);
    vkFreeMemory(device, textureMemory, nullptr);
}

// TODO Format will be used for depth buffer, do not remove
SingleUseCommandBuffer Texture::TransitionImageLayout(VkFormat Format, const VkImageLayout NewLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = layout;
    barrier.newLayout = NewLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texture;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    CommandPool *command_pool;
    if (layout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        command_pool = GraphicsEngine::Get().memoryCommandPool;
    }
    else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        command_pool = GraphicsEngine::Get().renderCommandPool;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    SingleUseCommandBuffer command_buffer(*command_pool);
    vkCmdPipelineBarrier(
        command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    layout = NewLayout;
    return command_buffer;
}

void Texture::CopyBuffer(const Buffer &Buffer, const bool TransitionToShaderUse)
{
    SingleUseCommandBuffer command_buffer = TransitionImageLayout(
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = extent;

	vkCmdCopyBufferToImage(command_buffer,
	                       Buffer,
	                       this->texture,
	                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	                       1,
	                       &region);

    command_buffer.Submit();


    if (TransitionToShaderUse) {
        TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}
