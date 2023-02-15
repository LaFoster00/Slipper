#pragma once
#include "signal.h"
#include <iostream>
#include <stdexcept>

#ifdef LINUX
#    define SLIPPER_FUNCTION __attribute__((dllexport))
#    define MAX_PATH PATH_MAX
#    define DEBUG_BREAK raise(SIGTRAP);
#elif WINDOWS
#    ifdef LIBRARY_EXPORT
#        define SLIPPER_API _declspec(dllexport)
#    else
#        define SLIPPER_API _declspec(dllimport)
#    endif
#    define DEBUG_BREAK __debugbreak();
#else
#    define SLIPPER_FUNCTION
#endif

#include "Util/StringUtil.h"

#define VK_ASSERT(func, message)                                                            \
    if (func != VK_SUCCESS) {                                                               \
        std::cout << "\nA Vulkan exepction occured at line: '" << __LINE__ << "' in file '" \
                  << __FILE__ << "':\n";                                                    \
        std::string formatedMessage = "\n\t";                                               \
        formatedMessage += message;                                                         \
        formatedMessage += '\n';                                                            \
        DEBUG_BREAK                                                                         \
        throw std::runtime_error(formatedMessage);                                          \
    }

#define LOG(message) std::cout << message;

#define ASSERT(statement, ...)                                                                \
    if (statement) {                                                                          \
        std::cout << "\nAn error occured at line: '" << __LINE__ << "' in file '" << __FILE__ \
                  << "':\n";                                                                  \
        std::string formatedMessage = "\n\t";                                                 \
        formatedMessage += String::append(__VA_ARGS__);                                       \
        formatedMessage += '\n';                                                              \
        DEBUG_BREAK                                                                           \
        throw std::runtime_error(formatedMessage);                                            \
    }

#ifdef __cplusplus
#    define EXTERNC extern "C"
#else
#    define EXTERNC
#endif