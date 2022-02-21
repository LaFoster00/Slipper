#pragma once

#ifdef __GNUC__
#define SLIPPER_FUNCTION __attribute__((dllexport))
#elif _MSC_VER
#define SLIPPER_FUNCTION _declspec(dllexport)
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
