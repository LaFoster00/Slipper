#pragma once

#ifdef __GNUC__
#define LINUX
#define SLIPPER_FUNCTION __attribute__((dllexport))
#elif _MSC_VER
#define WINDOWS
#define SLIPPER_FUNCTION _declspec(dllexport)
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif