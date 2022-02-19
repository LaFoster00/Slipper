#include <iostream>
#include <common_defines.h>

EXTERNC
{
    SLIPPER_FUNCTION void say_hello()
    {
        std::cout << "Hello, from Slipper_Engine!\n";
    }
}