#include <cstring>
#include <filesystem>
#include <iostream>

#include "Tutorial/HelloTriangleApplication.h"

EXTERNC
{
    SLIPPER_FUNCTION int main(int argc, char *argv[])
    {
	    try
	    {
		    HelloTriangleApplication app;
		    app.Run();
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}