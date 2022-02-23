#pragma once

#include "common_includes.h"
#include "Instance.h"

class Surface
{
public:
    void Create(const Instance *instance, GLFWwindow *window);
    inline void Destroy()
    {
        vkDestroySurfaceKHR(instance->instance, surface, nullptr);
    }

public:
    Instance *instance;
    VkSurfaceKHR surface;
};