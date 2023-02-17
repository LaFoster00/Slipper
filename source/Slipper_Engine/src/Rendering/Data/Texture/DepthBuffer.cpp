#include "DepthBuffer.h"

#include "Setup/GraphicsSettings.h"

namespace Slipper
{
DepthBuffer::DepthBuffer(const VkExtent2D Extent, const VkFormat Format)
    : Texture(VK_IMAGE_TYPE_2D,
              VkExtent3D(Extent.width, Extent.height, 1),
              Format,
              {},
              false,
              GraphicsSettings::Get().MSAA_SAMPLES,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
              VK_IMAGE_ASPECT_DEPTH_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
}
}  // namespace Slipper
