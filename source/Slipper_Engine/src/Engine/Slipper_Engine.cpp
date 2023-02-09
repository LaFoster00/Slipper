#include "Slipper_Engine.h"

#include <iostream>

#include "Core/Application.h"

EXTERNC
{
    SLIPPER_FUNCTION int main(int argc, char *argv[])
    {
	    try
	    {
            Slipper::ApplicationInfo app_info{ "Slipper Engine " };
            auto app = new Slipper::SlipperEngine(app_info);
            app->Run();
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}