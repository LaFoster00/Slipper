cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SlipperEngine VERSION 0.1.0)

include(CTest)
enable_testing()

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

# Set the output folder where your program will be created
SetBuildDirectory()

option(SLIPPER_PRINTF_IN_SHADERS OFF)
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


# Fetch all source files
file(GLOB_RECURSE SLIPPER_ENGINE_SOURCE_FILES src/*.h src/*.cpp)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/helloworld")

message("Building Engine Library")
set(SLIPPER_ENGINE_NAME SlipperEngine CACHE STRING "Engine Name")
add_library(${SLIPPER_ENGINE_NAME} STATIC ${SLIPPER_ENGINE_SOURCE_FILES} ${TINY_OBJ_LOADER_FILES} ${SPIRV_REFLECT_FILES})

AddWholeArchiveFlag(${SLIPPER_ENGINE_NAME} SLIPPER_ENGINE_WHOLE)

target_compile_definitions(${SLIPPER_ENGINE_NAME} PRIVATE LIBRARY_EXPORT)

set_property(TARGET ${SLIPPER_ENGINE_NAME} PROPERTY CXX_STANDARD 20)

set(SLIPPER_ENGINE_INCLUDE_DIRECTORIES
    src
    src/Core
    src/Core/AppComponents
    src/Core/Input
    src/Ecs
    src/Ecs/Components
    src/Ecs/Systems
    src/Engine
    src/Filesystem
    src/Platform
    src/Rendering
    src/Rendering/Data
    src/Rendering/GraphicsPipeline
    src/SceneObjects
    src/Tutorial
    src/Window
)
GroupSources(src Source)
target_include_directories(${SLIPPER_ENGINE_NAME} PUBLIC
    ${SLIPPER_ENGINE_INCLUDE_DIRECTORIES} 
    ${ENTT_INCLUDE_DIR})

#Make engine dependent on engine content
add_dependencies(${SLIPPER_ENGINE_NAME} SlipperEngine_EngineContent)

set(COMMON_ENGINE_LINK_LIBRARIES
    glfw
    IMGUI
    ${Vulkan_LIBRARIES}
    EnTT::EnTT
)

#Libs
if(LINUX)
    target_link_libraries(${SLIPPER_ENGINE_NAME} ${COMMON_ENGINE_LINK_LIBRARIES} ${LIBSTDCXX_LIBRARIES} dl pthread X11)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(${SLIPPER_ENGINE_NAME} ${COMMON_ENGINE_LINK_LIBRARIES} User32.lib)
    else()
        target_link_libraries(${SLIPPER_ENGINE_NAME} ${COMMON_ENGINE_LINK_LIBRARIES} User32.lib msvcrt.lib)
    endif()
endif()

set(ENGINE_PRECOMPILE_HEADER ${CMAKE_CURRENT_LIST_DIR}/src/pch.h CACHE INTERNAL "")

target_precompile_headers(${SLIPPER_ENGINE_NAME} PUBLIC
    ${ENGINE_PRECOMPILE_HEADER}
)

#Append external source files to project
#Vulkan
target_include_directories(${SLIPPER_ENGINE_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
#glm
target_include_directories(${SLIPPER_ENGINE_NAME} PUBLIC "${EXTERNAL_LIBS_DIR}/glm")
#stb
target_include_directories(${SLIPPER_ENGINE_NAME} PUBLIC "${EXTERNAL_LIBS_DIR}/stb")
#magin_enum
target_include_directories(${SLIPPER_ENGINE_NAME} PUBLIC "${EXTERNAL_LIBS_DIR}/magic_enum/include")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)