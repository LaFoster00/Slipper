cmake_minimum_required(VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SlipperEngine VERSION 0.1.0)

include(CTest)
enable_testing()

string(CONCAT BUILD_TYPE ${CMAKE_BUILD_TYPE})
message("Building with ${BUILD_TYPE} config!")

set(BUILD_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>)
set(EXTERNAL_LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../extern")


if ("${BUILD_TYPE}" STREQUAL "Debug")
    if(LINUX)
        set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
        set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
    else()
        set(CMAKE_C_FLAGS_DEBUG "-DDEBUG")
        set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
    endif()
endif()

option(PRINTF_IN_SHADERS OFF)
if(PRINTF_IN_SHADERS)
add_compile_definitions(SHADER_PRINTF_ENABLED)
endif()

set(TARGET_INCLUDE_DIRECTORIES
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
include_directories(${TARGET_INCLUDE_DIRECTORIES})

# Fetch all source files
file(GLOB_RECURSE SLIPPER_ENGINE_SOURCE_FILES "*.h" "*.cpp") 

# Special thanks to Luca Gherardi who wrote this special little peace of code :)
function(GroupSources curdir rootfiledir rootsolutiondir)
    file(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR}/${curdir} ${CMAKE_CURRENT_LIST_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
            GroupSources(${curdir}/${child} ${rootfiledir} ${rootsolutiondir})
        else()
            if(NOT ${child} STREQUAL CMakeLists.txt)
                string(REPLACE "/" "\\" groupname ${curdir})
                #message("Child dir ${groupname}")
                #message("Child entry ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child}")
                # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)
                string(REPLACE "${rootfiledir}" "${rootsolutiondir}" groupname ${groupname})
                #message("Groupname entry ${groupname}")
                source_group(${groupname} FILES ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
            endif()
        endif()
    endforeach()
endfunction()

GroupSources(src src Source)

#Append external source files to project
#SPIRV-Reflect
list(APPEND SLIPPER_ENGINE_SOURCE_FILES 
    "${EXTERNAL_LIBS_DIR}/SPIRV-Reflect/spirv_reflect.h"
    "${EXTERNAL_LIBS_DIR}/SPIRV-Reflect/spirv_reflect.cpp")
include_directories(${EXTERNAL_LIBS_DIR}/SPIRV-Reflect)
source_group(Spirv-Reflect FILES 
    "${EXTERNAL_LIBS_DIR}/SPIRV-Reflect/spirv_reflect.h"
    "${EXTERNAL_LIBS_DIR}/SPIRV-Reflect/spirv_reflect.cpp")

#tinyobjloader
list(APPEND SLIPPER_ENGINE_SOURCE_FILES
    "${EXTERNAL_LIBS_DIR}/tinyobjloader/tiny_obj_loader.h"
    "${EXTERNAL_LIBS_DIR}/tinyobjloader/tiny_obj_loader.cc")
include_directories(${EXTERNAL_LIBS_DIR}/tinyobjloader)
source_group(tinyobjloader FILES 
    "${EXTERNAL_LIBS_DIR}/tinyobjloader/tiny_obj_loader.h"
    "${EXTERNAL_LIBS_DIR}/tinyobjloader/tiny_obj_loader.cc")

#Include Engine Content containing shader handling
set(COMPILE_SHADERS ON CACHE BOOL "Compile Shaders")
add_subdirectory(EngineContent)


option(OUTPUT_LIB OFF)
if(OUTPUT_LIB)
    message("Building Engine Library")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_library(SlipperEngine SHARED ${SLIPPER_ENGINE_SOURCE_FILES})
else()
    message("Building Engine Executable")    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_executable(SlipperEngine ${SLIPPER_ENGINE_SOURCE_FILES})
endif()

add_dependencies(SlipperEngine SlipperEngine_EngineContent)

set_property(TARGET SlipperEngine PROPERTY CXX_STANDARD 20)


#Libs
if(LINUX)
    target_link_libraries(SlipperEngine glfw3 dl pthread X11)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(SlipperEngine User32.lib glfw3)
    else()
        target_link_libraries(SlipperEngine User32.lib glfw3 msvcrt.lib)
    endif()

    
    #if (NOT OUTPUT_LIB)
    #    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SlipperEngine)
    #endif()
endif()

#include Vulkan
find_package(Vulkan)
if(NOT ${Vulkan_FOUND})
    message( FATAL_ERROR "Vulkan was not found!")
endif()
include_directories(SlipperEngine ${Vulkan_INCLUDE_DIRS})
target_link_libraries(SlipperEngine ${Vulkan_LIBRARIES})


#glfw
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/glfw/include")
target_link_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/glfw/build/src/")

#glm
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/glm")

#stb
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/stb")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)