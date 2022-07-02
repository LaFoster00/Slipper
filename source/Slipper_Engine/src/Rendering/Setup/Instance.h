#pragma once

#include "common_defines.h"
#include "common_includes.h"
#include <vector>

class Instance
{
 public:
	Instance();

	~Instance()
    {
        vkDestroyInstance(instance, nullptr);
    }

    static Instance &Get()
	{
        if (!m_instance)
        {
            m_instance = new Instance();
        }
        return *m_instance;
	}

    operator VkInstance()
	{
		return instance;
	}

 private:
	static bool CheckValidationLayerSupport();
	static std::vector<const char *> GetRequiredExtensions();

 public:
    VkInstance instance;

private:
    static Instance *m_instance;
};