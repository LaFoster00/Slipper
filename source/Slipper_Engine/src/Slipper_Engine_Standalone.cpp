#include "iostream"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(640, 480, "Slipper Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
