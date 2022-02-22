#include <iostream>
#include "common_defines.h"
#include <cstring>
#include <filesystem>

#define PRINT_CREATION_DETAILS
#define ENABLE_VALIDATION_LAYERS
#include "Tutorial/HelloTriangleApplication.h"

EXTERNC
{
    SLIPPER_FUNCTION int main(int argc, char *argv[])
    {
        HelloTriangleApplication app;

        try
        {
            app.run();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}