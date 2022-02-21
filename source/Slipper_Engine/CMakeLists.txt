cmake_minimum_required(VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Slipper_Engine VERSION 0.1.0)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
set(BUILD_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin/Engine")

if ("${BUILD_TYPE}" STREQUAL "debug")
    set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
endif()

include(CTest)
enable_testing()

message("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

include_directories(src)

option(LIB_COMPILE ON)
if(LIB_COMPILE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_library(Slipper_Engine SHARED src/Slipper_Engine.cpp)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_executable(Slipper_Engine src/Slipper_Engine_Standalone.cpp)
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
