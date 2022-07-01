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
    Window(WindowInfo createInfo);
    ~Window()
    {
        glfwDestroyWindow(glfwWindow);
    }

    bool ShouldClose()
    {
        return glfwWindowShouldClose(glfwWindow);
    }

    operator GLFWwindow *() const
    {
        return glfwWindow;
    }

 public:
    GLFWwindow *glfwWindow;

 private:
    WindowInfo info;
};