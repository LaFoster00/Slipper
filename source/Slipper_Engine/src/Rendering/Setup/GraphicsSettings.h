#pragma once

namespace Slipper
{
struct GraphicsSettings
{
    static GraphicsSettings &Get()
    {
        if (!m_instance) {
            m_instance = new GraphicsSettings();
        }

        return *m_instance;
    }

 public:
    VkSampleCountFlagBits MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;

 private:
    static GraphicsSettings *m_instance;
};
}  // namespace Slipper