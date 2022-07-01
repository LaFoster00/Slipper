#include "Window.h"

Window::Window(WindowInfo createInfo)
{
    info = createInfo;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, createInfo.resizable);

    glfwWindow = glfwCreateWindow(info.width, info.height, info.name, nullptr, nullptr);
}