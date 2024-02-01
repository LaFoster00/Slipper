#pragma once

namespace Slipper
{
class VKDevice;
class RenderPass;
class SwapChain;

class Framebuffer : DeviceDependentObject
{
 public:
    Framebuffer(const RenderPass *RenderPass,
                const VkImageView *Attachments,
                uint32_t AttachmentCount,
                VkExtent2D Extent);
    ~Framebuffer();

    operator VkFramebuffer() const
    {
        return vkFramebuffer;
    }

 private:
    void Create(const RenderPass *RenderPass,
                const VkImageView *Attachments,
                uint32_t AttachmentCount,
                VkExtent2D Extent);

 public:
    VkFramebuffer vkFramebuffer;
};
}  // namespace Slipper