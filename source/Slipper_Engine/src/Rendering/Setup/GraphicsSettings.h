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
    vk::SampleCountFlagBits MSAA_SAMPLES = vk::SampleCountFlagBits::e1;

 private:
    static GraphicsSettings *m_instance;
};
}  // namespace Slipper