cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Slipper_Engine VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(src)

add_library(Slipper_Engine SHARED src/Slipper_Engine.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
