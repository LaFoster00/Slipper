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

set(SLIPPER_ENGINE_SOURCE_FILES
    src/Engine/Slipper_Engine.cpp
    src/terminal_util.cpp
    src/Filesystem/File.cpp
    
    src/Tutorial/HelloTriangleApplication.cpp
    
    src/Window/Window.cpp
    
    src/VulkanApp/Setup/Instance.cpp
    src/VulkanApp/Setup/Device.cpp
    src/VulkanApp/Presentation/Surface.cpp
    src/VulkanApp/Presentation/SwapChain.cpp
    
    src/VulkanApp/GraphicsEngine.cpp
    src/VulkanApp/GraphicsPipeline/Shader.cpp
    src/VulkanApp/GraphicsPipeline/GraphicsPipeline.cpp
    src/VulkanApp/GraphicsPipeline/RenderPass.cpp
    src/VulkanApp/GraphicsPipeline/PipelineLayout.cpp

    src/VulkanApp/Drawing/Framebuffer.cpp
    src/VulkanApp/Drawing/CommandPool.cpp
)

set(SLIPPER_ENGINE_HEADER_FILES
    src/Tutorial/HelloTriangleApplication.h

    src/common_defines.h
    src/common_includes.h
    src/terminal_util.h

    src/Engine/Engine.h
    src/Filesystem/File.h
    src/Filesystem/Path.h

    src/VulkanApp/Setup/Instance.h
    src/VulkanApp/Setup/Device.h
    src/VulkanApp/Presentation/Surface.h
    src/VulkanApp/Presentation/SwapChain.h
    
    src/VulkanApp/GraphicsEngine.h
    src/VulkanApp/GraphicsPipeline/Shader.h
    src/VulkanApp/GraphicsPipeline/GraphicsPipeline.h
    src/VulkanApp/GraphicsPipeline/RenderPass.h
    src/VulkanApp/GraphicsPipeline/PipelineLayout.h

    src/VulkanApp/Drawing/Framebuffer.h
    src/VulkanApp/Drawing/CommandPool.h
)

option(OUTPUT_LIB OFF)
if(OUTPUT_LIB)
    message("Building Engine Library")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_library(Slipper_Engine SHARED ${SLIPPER_ENGINE_SOURCE_FILES} ${SLIPPER_ENGINE_HEADER_FILES})
else()
    message("Building Engine Executable")    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
    add_executable(Slipper_Engine ${SLIPPER_ENGINE_SOURCE_FILES} ${SLIPPER_ENGINE_HEADER_FILES})
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