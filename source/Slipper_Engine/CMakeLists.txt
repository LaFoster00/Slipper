cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SlipperEngine VERSION 0.1.0)

include(CTest)
enable_testing()

set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>)
# Set the output folder where your program will be created
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

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

# Fetch all source files
file(GLOB_RECURSE SLIPPER_ENGINE_SOURCE_FILES src/*.h src/*.cpp)

message("Building Engine Library")
add_library(SlipperEngine STATIC ${SLIPPER_ENGINE_SOURCE_FILES} ${IMGUI_FILES} ${IMGUIZMO_FILES} ${TINY_OBJ_LOADER_FILES} ${SPIRV_REFLECT_FILES})

# Fetch all source files
file(GLOB_RECURSE SLIPPER_ENGINE_TEST_SOURCE_FILES test/*.h test/*.cpp)

message("Building Engine Executable")
add_executable(SlipperEngine_Test ${SLIPPER_ENGINE_TEST_SOURCE_FILES})

set_property(TARGET SlipperEngine PROPERTY CXX_STANDARD 20)
set_property(TARGET SlipperEngine_Test PROPERTY CXX_STANDARD 20)

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
target_include_directories(SlipperEngine PUBLIC ${SLIPPER_ENGINE_INCLUDE_DIRECTORIES})

GroupSources(test test Source)
target_include_directories(SlipperEngine_Test PUBLIC test)
#target_include_directories(SlipperEngine_Test PRIVATE ${SLIPPER_ENGINE_INCLUDE_DIRECTORIES})

#Make engine dependent on engine content
add_dependencies(SlipperEngine SlipperEngine_EngineContent)
add_dependencies(SlipperEngine_Test SlipperEngine)

set(COMMOND_LINK_LIBRARIES glfw)

#Libs
if(LINUX)
    target_link_libraries(SlipperEngine ${COMMOND_LINK_LIBRARIES} dl pthread X11)
    target_link_libraries(SlipperEngine_Test Slipper_Engine dl pthread)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(SlipperEngine ${COMMOND_LINK_LIBRARIES} User32.lib)
        target_link_libraries(SlipperEngine_Test SlipperEngine User32.lib)
    else()
        target_link_libraries(SlipperEngine ${COMMOND_LINK_LIBRARIES} User32.lib msvcrt.lib)
        target_link_libraries(SlipperEngine_Test SlipperEngine User32.lib msvcrt.lib)
    endif()
endif()

target_compile_definitions(SlipperEngine PRIVATE LIBRARY_EXPORT)
target_precompile_headers(SlipperEngine PUBLIC
    src/pch.h
)

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
target_include_directories(SlipperEngine_Test PUBLIC "${EXTERNAL_LIBS_DIR}/glm")
#stb
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/stb")
target_include_directories(SlipperEngine_Test PUBLIC "${EXTERNAL_LIBS_DIR}/stb")
#imgui
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/imgui")
target_include_directories(SlipperEngine_Test PUBLIC "${EXTERNAL_LIBS_DIR}/imgui")
#ImGuizmo
target_include_directories(SlipperEngine PUBLIC "${EXTERNAL_LIBS_DIR}/ImGuizmo")
target_include_directories(SlipperEngine_Test PUBLIC "${EXTERNAL_LIBS_DIR}/ImGuizmo")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)