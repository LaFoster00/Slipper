#pragma once
#include <stdexcept>

#ifdef __GNUC__
#define LINUX
#define SLIPPER_FUNCTION __attribute__((dllexport))
#elif _MSC_VER
#define WINDOWS
#define SLIPPER_FUNCTION _declspec(dllexport)
#endif

#define VK_ASSERT(func, message)           \
    if (func != VK_SUCCESS)                \
    {                                      \
        throw std::runtime_error(message); \
    }

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif