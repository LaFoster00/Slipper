#pragma once

#define SLIPPER_FUNCTION __attribute__((dllexport))

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
