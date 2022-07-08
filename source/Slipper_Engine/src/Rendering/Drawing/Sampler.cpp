#include "Sampler.h"

#include "Setup/Device.h"
#include "common_defines.h"

Sampler *Sampler::m_linearSampler = nullptr;
Sampler *Sampler::m_nearestSampler = nullptr;

Sampler::Sampler(const VkFilter Filter, const VkSamplerAddressMode AddressMode)
{
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = Filter;
    sampler_info.minFilter = Filter;
    sampler_info.addressModeU = AddressMode;
    sampler_info.addressModeV = AddressMode;
    sampler_info.addressModeW = AddressMode;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = device.deviceProperties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    VK_ASSERT(vkCreateSampler(device, &sampler_info, nullptr, &sampler),
              "Failed to create texture sampler!")
}

Sampler::~Sampler()
{
    vkDestroySampler(device, sampler, nullptr);
}