#include "Surface.h"

#include "common_defines.h"
#include "Setup/Device.h"
#include "Setup/Instance.h"

Surface::Surface(GLFWwindow *window)
{
    VK_ASSERT(glfwCreateWindowSurface(Instance::Get(), window, nullptr, &surface),
              "Failed to create window suface!");
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(Instance::Get(), surface, nullptr);
}