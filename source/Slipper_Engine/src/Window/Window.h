#pragma once

#include "common_includes.h"

struct WindowInfo
{
   public:
    const char *name;
    uint32_t width;
    uint32_t height;
    bool resizable;
};

class Window
{
   public:
    GLFWwindow *CreateWindow(WindowInfo createInfo);

    bool ShouldClose()
    {
        return glfwWindowShouldClose(glfwWindow);
    }

    void Destroy()
    {
        glfwDestroyWindow(glfwWindow);
    }

   public:
    GLFWwindow *glfwWindow;

   private:
    WindowInfo info;
};