#include "Surface.h"
#include "Device.h"
#include "Instance.h"

void Surface::Create(const Instance *instance, GLFWwindow *window)
{
    this->instance = instance;
    VK_ASSERT(glfwCreateWindowSurface(instance->instance, window, nullptr, &surface), "Failed to create window suface!");
}

void Surface::Destroy()
{
    vkDestroySurfaceKHR(instance->instance, surface, nullptr);
}