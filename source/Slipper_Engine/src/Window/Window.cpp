#include "Window.h"

GLFWwindow *Window::CreateWindow(WindowInfo createInfo)
{
    info = createInfo;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, createInfo.resizable);

    window = glfwCreateWindow(info.width, info.height, info.name, nullptr, nullptr);

    return window;
}