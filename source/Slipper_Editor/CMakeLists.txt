cmake_minimum_required(VERSION 3.24.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SlipperEditor VERSION 0.1.0)

include(CTest)
enable_testing()

string(CONCAT BUILD_TYPE ${CMAKE_BUILD_TYPE})
message("Building with ${BUILD_TYPE} config!")

if ("${BUILD_TYPE}" STREQUAL "Debug")
    if(LINUX)
        set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
        set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
    else()
        set(CMAKE_C_FLAGS_DEBUG "-DDEBUG")
        set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
    endif()
endif()

# Set the output folder where your program will be created
SetBuildDirectory()

message("Building Engine Executable")    
# Fetch all source files
file(GLOB_RECURSE SLIPPER_EDITOR_SOURCE_FILES "*.h" "*.cpp") 
set(SLIPPER_EDITOR_NAME SlipperEditor CACHE STRING "Engine Name")
add_executable(${SLIPPER_EDITOR_NAME} ${SLIPPER_EDITOR_SOURCE_FILES})

set_property(TARGET ${SLIPPER_EDITOR_NAME} PROPERTY CXX_STANDARD 20)

#Make engine dependent on engine content
add_dependencies(${SLIPPER_EDITOR_NAME} ${SLIPPER_ENGINE_NAME})

set(SLIPPER_EDITOR_INCLUDE_DIRECTORIES
    src
    src/EditorAppComponents
    src/Editors
    src/Editors/ComponentEditors
    src/Ecs
    src/Ecs/Components
    src/Ecs/Systems
)
GroupSources(src Source)

target_include_directories(${SLIPPER_EDITOR_NAME} PRIVATE ${SLIPPER_EDITOR_INCLUDE_DIRECTORIES})
target_precompile_headers(${SLIPPER_EDITOR_NAME} PUBLIC ${ENGINE_PRECOMPILE_HEADER})

set(COMMON_EDITOR_LIBS
    ${SLIPPER_ENGINE_WHOLE}
)

#Libs
if(LINUX)
    target_link_libraries(${SLIPPER_EDITOR_NAME} ${COMMON_EDITOR_LIBS} ${LIBSTDCXX_LIBRARIES} dl pthread)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(${SLIPPER_EDITOR_NAME} ${COMMON_EDITOR_LIBS} User32.lib)
    else()
        target_link_libraries(${SLIPPER_EDITOR_NAME} ${COMMON_EDITOR_LIBS} User32.lib msvcrt.lib)
    endif()
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)