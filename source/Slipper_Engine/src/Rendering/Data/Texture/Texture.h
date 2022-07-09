#pragma once

#include <memory>

#include "common_includes.h"

#include "DeviceDependentObject.h"
#include "Drawing/CommandPool.h"
#include "Drawing/Sampler.h"

class Sampler;
class Buffer;

class Texture : DeviceDependentObject
{
 public:
    Texture(VkImageType Type,
            VkExtent3D Extent,
            VkFormat Format,
            VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
            uint32_t ArrayLayers = 1);
    Texture(const Texture &Other) = delete;
    Texture(Texture &&Other) noexcept: sampler(std::move(Other.sampler)), arrayLayerCount(Other.arrayLayerCount), imageInfo(std::move(Other.imageInfo))
    {
	    texture = Other.texture;
	    Other.texture = VK_NULL_HANDLE;

	    textureMemory = Other.textureMemory;
	    Other.textureMemory = VK_NULL_HANDLE;

	    textureView = Other.textureView;
	    Other.textureView = VK_NULL_HANDLE;

	    type = Other.type;
	    extent = Other.extent;
	    format = Other.format;
	    imageAspect = Other.imageAspect;
    }

    virtual ~Texture();

    SingleUseCommandBuffer TransitionImageLayout(VkFormat Format, VkImageLayout NewLayout);
    void CopyBuffer(const Buffer &Buffer, bool TransitionToShaderUse = true);
    VkDescriptorImageInfo *GetDescriptorImageInfo() const;

    operator VkImage() const
    {
        return texture;
    }

    operator VkImageView() const
    {
        return textureView;
    }

    [[nodiscard]] static VkImageView CreateImageView(
        VkImage image,
        VkImageType Type,
        VkFormat Format,
        VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
        uint32_t ArrayLayerCount = 1);

 public:
    VkImage texture;
    VkDeviceMemory textureMemory;
    Sampler sampler;

    VkImageView textureView;

    VkImageType type;
    VkExtent3D extent;
    VkFormat format;
    
    VkImageAspectFlags imageAspect;
    uint32_t arrayLayerCount;

    std::unique_ptr<VkDescriptorImageInfo> imageInfo;
};
