#pragma once

#include "common_includes.h"

class Device;

class RenderPass
{
public:
    void Create(Device *device);
    void Destroy();

public:
    Device *owningDevice;

    VkRenderPass vkRenderPass;
};