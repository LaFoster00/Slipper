#pragma once

#ifdef LINUX
#    ifdef LIBRARY_EXPORT
#        define SLIPPER_API __attribute__((dllexport))
#    else
#        define SLIPPER_API __attribute__((dllimport))
#    endif
#elif WINDOWS
#    ifdef LIBRARY_EXPORT
#        define SLIPPER_API _declspec(dllexport)
#    else
#        define SLIPPER_API _declspec(dllimport)
#    endif
#endif