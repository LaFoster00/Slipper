#pragma once

#include "DeviceDependentObject.h"
#include "common_includes.h"

class Sampler : DeviceDependentObject
{
 public:
    Sampler(VkFilter Filter, VkSamplerAddressMode AddressMode);
    Sampler(const Sampler &Other) = delete;
    Sampler(Sampler &&Other)
    {
        sampler = Other.sampler;
        Other.sampler = VK_NULL_HANDLE;
    }
    ~Sampler();

    operator VkSampler() const
    {
        return sampler;
    }

 public:
    VkSampler sampler;
};