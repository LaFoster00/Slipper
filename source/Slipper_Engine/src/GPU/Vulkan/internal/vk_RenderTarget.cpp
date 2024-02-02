#include "../vk_RenderTarget.h"

#include "Setup/GraphicsSettings.h"

namespace Slipper
{
RenderTarget::RenderTarget(const VkExtent2D Extent, const vk::Format Format)
    : Texture(vk::ImageType::e2D,
              VkExtent3D(Extent.width, Extent.height, 1),
              Format,
              {},
              false,
              GraphicsSettings::Get().MSAA_SAMPLES,
              vk::ImageTiling::eOptimal,
              vk::ImageUsageFlagBits::eTransientAttachment |
                  vk::ImageUsageFlagBits::eColorAttachment)
{
}
}  // namespace Slipper
