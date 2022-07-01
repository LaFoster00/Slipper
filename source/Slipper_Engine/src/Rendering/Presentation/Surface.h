#pragma once

#include "common_includes.h"

class Instance;
class Device;
class Window;

class Surface
{
 public:
    Surface(GLFWwindow *window);
    ~Surface();

    operator VkSurfaceKHR()
    {
        return surface;
    }

 private:
 public:
    VkSurfaceKHR surface;
};