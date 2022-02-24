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
    set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
endif()

include_directories(src)

set(SLIPPER_ENGINE_SOURCE_FILES 
    src/Engine/Slipper_Engine.cpp
    src/Tutorial/HelloTriangleApplication.cpp
    src/Window/Window.cpp
    src/VulkanApp/Instance.cpp
    src/VulkanApp/Device.cpp
    src/VulkanApp/Surface.cpp
    src/terminal_util.cpp
    src/VulkanApp/SwapChain.cpp
    src/Filesystem/File.cpp
    src/VulkanApp/Shader.cpp
    src/VulkanApp/GraphicsPipeline.cpp
    src/VulkanApp/RenderPass.cpp
    src/VulkanApp/Framebuffer.cpp
    src/VulkanApp/CommandBuffer/CommandPool.cpp
)

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
target_link_libraries(Slipper_Engine glfw3 dl pthread X11)


#glm
target_include_directories(Slipper_Engine PUBLIC "${EXTERNAL_LIBS_DIR}/glm")

#bgfx
#include_directories(${EXTERNAL_LIBS_DIR}/bgfx/include)
#if (UNIX)
#    target_link_directories(Slipper_Engine PRIVATE "${EXTERNAL_LIBS_DIR}/bgfx/.build/linux64_gcc/bin/")
#    if (${LIB_DEBUG})
#        if (NOT EXISTS "${EXTERNAL_LIBS_DIR}/bgfx/.build/linux64_gcc/bin/libbgfxDebug.a")
#            message(WARNING "The bgfx submodule was not built as debug! Go to bgfx folder and run 'make linux-debug64' or build the make file in the repo root directory!")
#        endif()
#        target_link_libraries(Slipper_Engine bgfxDebug bimgDebug bxDebug)
#    else()
#        if (NOT EXISTS "${EXTERNAL_LIBS_DIR}/bgfx/.build/linux64_gcc/bin/libbgfxRelease.a")
#            message(WARNING "The bgfx submodule was not built as release! Go to bgfx folder and run 'make linux-release64' or build the make file in the repo root directory!")
#        endif()
#        target_link_libraries(Slipper_Engine bgfxRelease bimgRelease bxRelease)
#    endif()
#elseif(WIN32)
#    message(FATAL_ERROR "Setup bgfx for windows")
#endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)