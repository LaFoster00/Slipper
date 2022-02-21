cmake_minimum_required(VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Slipper_Engine VERSION 0.1.0)

include(CTest)
enable_testing()

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
set(BUILD_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin/Engine")
set(EXTERNAL_LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../extern")

if ("${BUILD_TYPE}" STREQUAL "debug")
    set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
endif()

include_directories(src)

option(OUTPUT_LIB OFF)
if(OUTPUT_LIB)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_library(Slipper_Engine SHARED src/Slipper_Engine.cpp)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_executable(Slipper_Engine src/Slipper_Engine_Standalone.cpp)
endif()

option(LIB_DEBUG OFF)


#Libs

#glfw
target_include_directories(Slipper_Engine PRIVATE "${EXTERNAL_LIBS_DIR}/glfw/include")
target_link_directories(Slipper_Engine PRIVATE "${EXTERNAL_LIBS_DIR}/glfw/build/src/")
message("${EXTERNAL_LIBS_DIR}/glfw/build/src/")
target_link_libraries(Slipper_Engine glfw3 dl pthread X11)


#include Vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
target_include_directories(Slipper_Engine PRIVATE ${VULKAN_INCLUDE_DIRS})
target_link_libraries(Slipper_Engine ${VULKAN_LIB_LIST})
target_link_libraries(Slipper_Engine GL)

#bgfx
include_directories(${EXTERNAL_LIBS_DIR}/bgfx/include)
if (UNIX)
    target_link_directories(Slipper_Engine PRIVATE "${EXTERNAL_LIBS_DIR}/bgfx/.build/linux64_gcc/bin/")
    if (${LIB_DEBUG})
        if (NOT EXISTS "${EXTERNAL_LIBS_DIR}/bgfx/.build/linux64_gcc/bin/libbgfxDebug.a")
            message(FATAL_ERROR "The bgfx submodule was not built as debug! Go to bgfx folder and run: make linux-debug64")
        endif()
        target_link_libraries(Slipper_Engine bgfxDebug bimgDebug bxDebug)
    else()
        if (NOT EXISTS "${EXTERNAL_LIBS_DIR}/bgfx/.build/linux64_gcc/bin/libbgfxRelease.a")
            message(FATAL_ERROR "The bgfx submodule was not built as release! Go to bgfx folder and run: make linux-release64")
        endif()
        target_link_libraries(Slipper_Engine bgfxRelease bimgRelease bxRelease)
    endif()
elseif(WIN32)
    message(FATAL_ERROR "Setup bgfx for windows")
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)