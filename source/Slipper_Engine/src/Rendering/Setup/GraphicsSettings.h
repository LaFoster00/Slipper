#pragma once

#include "Engine.h"

struct GraphicsSettings
{
    static GraphicsSettings &Get()
    {
        if (!m_instance)
        {
            m_instance = new GraphicsSettings();
        }

        return *m_instance;
    }

public:
    VkSampleCountFlagBits MSAA_SAMPLES;

private:
    static GraphicsSettings *m_instance;
};
