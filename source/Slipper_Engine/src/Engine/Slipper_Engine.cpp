#include <cstring>
#include <filesystem>
#include <iostream>

#include "Tutorial/HelloTriangleApplication.h"

EXTERNC
{
    SLIPPER_FUNCTION int main(int argc, char *argv[])
    {
        HelloTriangleApplication app;

        try {
            app.run();
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}