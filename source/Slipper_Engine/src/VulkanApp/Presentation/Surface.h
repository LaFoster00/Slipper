#pragma once

#include "common_includes.h"

class Instance;
class Device;
class Window;

class Surface
{
 public:
    void Create(const Instance *instance, GLFWwindow *window);
    void Destroy();

 private:
 public:
    const Instance *instance;
    VkSurfaceKHR surface;
};