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


target_compile_definitions(${SLIPPER_ENGINE_NAME} PRIVATE LIBRARY_EXPORT)

set_property(TARGET ${SLIPPER_ENGINE_NAME} PROPERTY CXX_STANDARD 20)

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
    src/Components
    src/SceneObjects
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

target_precompile_headers(${SLIPPER_ENGINE_NAME} PUBLIC
    src/pch.h
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


##### Test Application #####
message("Building Engine Executable")
# Fetch all source files
file(GLOB_RECURSE SLIPPER_ENGINE_TEST_SOURCE_FILES test/*.h test/*.cpp)

add_executable(SlipperEngine_Test ${SLIPPER_ENGINE_TEST_SOURCE_FILES})
add_dependencies(SlipperEngine_Test ${SLIPPER_ENGINE_NAME})

#target_precompile_headers(SlipperEngine_Test REUSE_FROM SlipperEngine)

set_property(TARGET SlipperEngine_Test PROPERTY CXX_STANDARD 20)

GroupSources(test Source)
target_include_directories(SlipperEngine_Test PUBLIC test)
target_include_directories(SlipperEngine_Test PRIVATE ${SLIPPER_ENGINE_INCLUDE_DIRECTORIES})

set(COMMON_EDITOR_LIBS
    ${SLIPPER_ENGINE_NAME}    
)

#Libs
if(LINUX)
    target_link_libraries(SlipperEngine_Test ${COMMON_EDITOR_LIBS} ${LIBSTDCXX_LIBRARIES} dl pthread)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(SlipperEngine_Test ${COMMON_EDITOR_LIBS} User32.lib)
    else()
        target_link_libraries(SlipperEngine_Test ${COMMON_EDITOR_LIBS} User32.lib msvcrt.lib)
    endif()
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)