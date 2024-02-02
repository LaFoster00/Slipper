#include "../vk_DepthBuffer.h"

#include "GraphicsSettings.h"


namespace Slipper::GPU::Vulkan
{
    DepthBuffer::DepthBuffer(const VkExtent2D Extent, const vk::Format Format)
        : Texture(vk::ImageType::e2D,
                  VkExtent3D(Extent.width, Extent.height, 1),
                  Format,
                  {},
                  false,
                  static_cast<vk::SampleCountFlagBits>(GraphicsSettings::MSAA_SAMPLES),
                  vk::ImageTiling::eOptimal,
                  vk::ImageUsageFlagBits::eDepthStencilAttachment,
                  vk::ImageAspectFlagBits::eDepth,
                  1)
    {
    }
}  // namespace Slipper
