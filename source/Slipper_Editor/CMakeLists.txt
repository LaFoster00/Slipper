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

# Fetch all source files
file(GLOB_RECURSE SLIPPER_EDITOR_SOURCE_FILES "*.h" "*.cpp") 

set(SLIPPER_EDITOR_INCLUDE_DIRECTORIES
    src
)
GroupSources(src src Source)

set(SLIPPER_ENGINE_INCLUDE_DIRECTORIES
    ../Slipper_Engine/src
)
include_directories(${SLIPPER_EDITOR_INCLUDE_DIRECTORIES} ${SLIPPER_ENGINE_INCLUDE_DIRECTORIES})

message("Building Engine Executable")    
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIRECTORY})
add_executable(SlipperEditor ${SLIPPER_EDITOR_SOURCE_FILES})

set_property(TARGET SlipperEditor PROPERTY CXX_STANDARD 20)

if (OUTPUT_LIB)
#Make engine dependent on engine content
add_dependencies(SlipperEditor SlipperEngine)
target_link_directories(SlipperEditor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../Slipper_Engine/bin/$<CONFIG>)
target_link_libraries(SlipperEditor SlipperEngine)
endif()

#Libs
if(LINUX)
    target_link_libraries(SlipperEditor dl pthread X11)
else()
    message("Linking for windows")    
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_libraries(SlipperEditor User32.lib)
    else()
        target_link_libraries(SlipperEditor User32.lib msvcrt.lib)
    endif()
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)