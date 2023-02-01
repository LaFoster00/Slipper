#pragma once

#include "DeviceDependentObject.h"

class Sampler : DeviceDependentObject
{
 public:
    Sampler(VkFilter Filter, VkSamplerAddressMode AddressMode);
    Sampler(const Sampler &Other) = delete;
    Sampler(Sampler &&Other) noexcept
    {
        sampler = Other.sampler;
        Other.sampler = VK_NULL_HANDLE;
    }
    ~Sampler();

    operator VkSampler() const
    {
        return sampler;
    }

    static void DestroyDefaultSamplers()
    {
        delete m_linearSampler;
        m_linearSampler = nullptr;
        delete m_nearestSampler;
        m_nearestSampler = nullptr;
    }

    static const Sampler &GetLinearSampler()
    {
        if (!m_linearSampler)
        {
            m_linearSampler = new Sampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        }
        return *m_linearSampler;
    }

    static const Sampler &GetNearestSampler()
    {
        if (!m_nearestSampler)
        {
            m_nearestSampler = new Sampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        }
        return *m_nearestSampler;
    }

 public:
    VkSampler sampler;

 private:
    static Sampler *m_linearSampler;
    static Sampler *m_nearestSampler;
};