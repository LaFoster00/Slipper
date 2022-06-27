cmake_minimum_required(VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Slipper_Engine VERSION 0.1.0)

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

include_directories(src)

file(GLOB_RECURSE SLIPPER_ENGINE_SOURCE_FILES "*.h" "*.cpp")

# Special thanks to Luca Gherardi who wrote this special little peace of code :)
macro(GroupSources curdir)
    file(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR}/${curdir} ${CMAKE_CURRENT_LIST_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
            GroupSources(${curdir}/${child})
        else()
            string(REPLACE "/" "\\" groupname ${curdir})
            message("Child dir ${groupname}")
            message("Child entry ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child}")
            # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)
            string(REPLACE "src" "Source" groupname ${groupname})
            source_group(${groupname} FILES ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
        endif()
    endforeach()
endmacro()

GroupSources(src)

option(OUTPUT_LIB OFF)
if(OUTPUT_LIB)
    message("Building Engine Library")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_library(Slipper_Engine SHARED ${SLIPPER_ENGINE_SOURCE_FILES})
else()
    message("Building Engine Executable")    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_executable(Slipper_Engine ${SLIPPER_ENGINE_SOURCE_FILES})
endif()

option(LIB_DEBUG OFF)


#Libs
#include Vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
include_directories(Slipper_Engine ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Slipper_Engine ${Vulkan_LIBRARIES})


#glfw
target_include_directories(Slipper_Engine PUBLIC "${EXTERNAL_LIBS_DIR}/glfw/include")
target_link_directories(Slipper_Engine PUBLIC "${EXTERNAL_LIBS_DIR}/glfw/build/src/")

if(LINUX)
    target_link_libraries(Slipper_Engine glfw3 dl pthread X11)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(Slipper_Engine User32.lib glfw3)
    else()
        target_link_libraries(Slipper_Engine User32.lib glfw3 msvcrt.lib)
    endif()

    
    #if (NOT OUTPUT_LIB)
    #    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Slipper_Engine)
    #endif()
endif()


#glm
target_include_directories(Slipper_Engine PUBLIC "${EXTERNAL_LIBS_DIR}/glm")

add_custom_command(TARGET Slipper_Engine PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_LIST_DIR}/EngineContent/ ${BUILD_OUTPUT_DIRECTORY}/EngineContent/)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)