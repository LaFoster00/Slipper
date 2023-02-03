cmake_minimum_required(VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SlipperEngine VERSION 0.1.0)

include(CTest)
enable_testing()

string(CONCAT BUILD_TYPE ${CMAKE_BUILD_TYPE})
message("Building with ${BUILD_TYPE} config!")

set(BUILD_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>)

if ("${BUILD_TYPE}" STREQUAL "Debug")
    if(LINUX)
        set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
        set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
    else()
        set(CMAKE_C_FLAGS_DEBUG "-DDEBUG")
        set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
    endif()
endif()

add_subdirectory(EngineContent)

option(PRINTF_IN_SHADERS OFF)
    if(PRINTF_IN_SHADERS)
    add_compile_definitions(SHADER_PRINTF_ENABLED)
    endif()

# Fetch all source files
file(GLOB_RECURSE SLIPPER_ENGINE_SOURCE_FILES "*.h" "*.cpp") 

set(SLIPPER_ENGINE_INCLUDE_DIRECTORIES
    src
    src/Engine
    src/Filesystem
    src/Rendering
    src/Rendering/Data
    src/Rendering/GraphicsPipeline
    src/Tutorial
    src/Window
    src/Platform
)
GroupSources(src src Source)
include_directories(${SLIPPER_ENGINE_INCLUDE_DIRECTORIES})

#SPIRV-Reflect
set(SPIRV_REFLECT_FILES 
    "${EXTERNAL_LIBS_DIR}/SPIRV-Reflect/spirv_reflect.h"
    "${EXTERNAL_LIBS_DIR}/SPIRV-Reflect/spirv_reflect.cpp")
include_directories(${EXTERNAL_LIBS_DIR}/SPIRV-Reflect)
source_group(extern/Spirv-Reflect FILES ${SPIRV_REFLECT_FILES})

#tinyobjloader
set(TINY_OBJ_LOADER_FILES
    "${EXTERNAL_LIBS_DIR}/tinyobjloader/tiny_obj_loader.h"
    "${EXTERNAL_LIBS_DIR}/tinyobjloader/tiny_obj_loader.cc")
include_directories(${EXTERNAL_LIBS_DIR}/tinyobjloader)
source_group(extern/tinyobjloader FILES ${TINY_OBJ_LOADER_FILES})

#imgui
source_group(extern/imgui FILES ${IMGUI_FILES})


option(OUTPUT_LIB OFF)
if(OUTPUT_LIB)
    message("Building Engine Library")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_library(SlipperEngine SHARED ${SLIPPER_ENGINE_SOURCE_FILES})
else()
    message("Building Engine Executable")    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_executable(SlipperEngine ${SLIPPER_ENGINE_SOURCE_FILES} ${IMGUI_FILES} ${TINY_OBJ_LOADER_FILES} ${SPIRV_REFLECT_FILES})
endif()

set_property(TARGET SlipperEngine PROPERTY CXX_STANDARD 20)

#Make engine dependent on engine content
add_dependencies(SlipperEngine SlipperEngine_EngineContent)

#Libs
if(LINUX)
    target_link_libraries(SlipperEngine glfw dl pthread X11)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(SlipperEngine User32.lib glfw)
    else()
        target_link_libraries(SlipperEngine User32.lib glfw msvcrt.lib)
    endif()
endif()

#Append external source files to project
#include Vulkan
find_package(Vulkan)
if(NOT ${Vulkan_FOUND})
    message( FATAL_ERROR "Vulkan was not found!")
endif()
target_link_libraries(SlipperEngine ${Vulkan_LIBRARIES})
include_directories(SlipperEngine ${Vulkan_INCLUDE_DIRS})

#glm
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/glm")
#stb
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/stb")
#imgui
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/imgui")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)