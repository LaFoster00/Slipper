#include "Surface.h"

void Surface::Create(const Instance *instance, GLFWwindow *window)
{
    VK_ASSERT(glfwCreateWindowSurface(instance->instance, window, nullptr, &surface), "Failed to create window suface!");
}