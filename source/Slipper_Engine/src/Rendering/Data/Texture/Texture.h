#pragma once

#include "common_includes.h"

#include <string>

#include "DeviceDependentObject.h"
#include "Drawing/CommandPool.h"


class Buffer;

class Texture : DeviceDependentObject
{
 public:
    Texture(VkImageType Type, VkExtent3D Extent, VkFormat Format);
    virtual ~Texture();

    SingleUseCommandBuffer TransitionImageLayout(VkFormat Format, VkImageLayout NewLayout);

    void CopyBuffer(const Buffer &Buffer, bool TransitionToShaderUse = true);

    operator VkImage&()
    {
        return texture;
    }

public:
    VkImage texture;
    VkDeviceMemory textureMemory;

    VkImageType type;
    VkExtent3D extent;
    VkFormat format;

    VkImageLayout layout;
};
