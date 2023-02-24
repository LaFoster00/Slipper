cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SlipperEngine VERSION 0.1.0)

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
add_executable(SlipperEditor ${SLIPPER_EDITOR_SOURCE_FILES})

set_property(TARGET SlipperEditor PROPERTY CXX_STANDARD 20)

#Make engine dependent on engine content
add_dependencies(SlipperEditor SlipperEngine)

set(SLIPPER_EDITOR_INCLUDE_DIRECTORIES
    src
    src/AppComponents
    src/Editors
)
GroupSources(src Source)

target_include_directories(SlipperEditor PRIVATE ${SLIPPER_EDITOR_INCLUDE_DIRECTORIES})
target_precompile_headers(SlipperEditor REUSE_FROM SlipperEngine)

set(COMMON_EDITOR_LIBS
    SlipperEngine    
)

#Libs
if(LINUX)
    target_link_libraries(SlipperEditor ${COMMON_EDITOR_LIBS} dl pthread)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(SlipperEditor ${COMMON_EDITOR_LIBS} User32.lib)
    else()
        target_link_libraries(SlipperEditor ${COMMON_EDITOR_LIBS} User32.lib msvcrt.lib)
    endif()
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)