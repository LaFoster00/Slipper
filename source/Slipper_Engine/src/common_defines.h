#pragma once
#include <iostream>
#include <stdexcept>

#ifdef LINUX
#    define SLIPPER_FUNCTION __attribute__((dllexport))
#    define MAX_PATH PATH_MAX
#elif WINDOWS
#    define SLIPPER_FUNCTION _declspec(dllexport)
#endif

#define VK_ASSERT(func, message)                                                            \
    if (func != VK_SUCCESS) {                                                               \
        std::cout << "\nA Vulkan exepction occured at line: '" << __LINE__ << "' in file '" \
                  << __FILE__ << "':\n";                                                    \
        std::string formatedMessage = "\n\t";                                               \
        formatedMessage += message;                                                         \
        formatedMessage += '\n';                                                            \
        throw std::runtime_error(formatedMessage);                                          \
    }

#define LOG(message) std::cout << message;

#define ASSERT(statement, message)                                                            \
    if (statement) {                                                                          \
        std::cout << "\nAn error occured at line: '" << __LINE__ << "' in file '" << __FILE__ \
                  << "':\n";                                                                  \
        std::string formatedMessage = "\n\t";                                                 \
        formatedMessage += message;                                                           \
        formatedMessage += '\n';                                                              \
        throw std::runtime_error(formatedMessage);                                            \
    }

#ifdef __cplusplus
#    define EXTERNC extern "C"
#else
#    define EXTERNC
#endif