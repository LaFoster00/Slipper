#include "RenderTarget.h"

#include "Setup/GraphicsSettings.h"

namespace Slipper
{
RenderTarget::RenderTarget(const VkExtent2D Extent, const VkFormat Format)
    : Texture(VK_IMAGE_TYPE_2D,
              VkExtent3D(Extent.width, Extent.height, 1),
              Format,
              {},
              false,
              GraphicsSettings::Get().MSAA_SAMPLES,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
{
}
}  // namespace Slipper
