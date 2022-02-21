cmake_minimum_required(VERSION 3.9.0)
project(Slipper VERSION 0.1.0)

include(CTest)
enable_testing()

if(UNIX AND NOT APPLE)
        set(LINUX TRUE)
endif()

string(CONCAT BUILD_TYPE ${CMAKE_BUILD_TYPE})

find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
#Udate submodule as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/extern/bgfx/makefile")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


set(EXTERNAL_LIB_DEBUG CACHE BOOL "Compile external libs with debug config.")

message(STATUS "Configuring BGFX")
if (LINUX)
    if(EXTERNAL_LIB_DEBUG)
        execute_process(
            COMMAND make linux-debug64
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/extern/bgfx
        )
    else()
        execute_process(
            COMMAND make linux-release64
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/extern/bgfx
        )
    endif()
elseif (WIN32)
    if(EXTERNAL_LIB_DEBUG)
        execute_process(
            COMMAND make mvs2017-debug64
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/extern/bgfx
        )
    else()
    execute_process(
        COMMAND make mvs2017-release64
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/extern/bgfx
    )
    endif()
endif()


#GLFW
message(STATUS "Configuring GLFW")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw ${CMAKE_CURRENT_LIST_DIR}/extern/glfw/build)


#Slipper
message(STATUS "Configuring SLIPPER_ENGINE Lib")
set(OUTPUT_LIB ON CACHE INTERNAL "")
add_subdirectory(source/Slipper_Engine)

message(STATUS "Configuring SLIPPER_EDITOR")
if (BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring Editor In Release Config")    
    add_custom_target(
        BuildEditor ALL
        COMMAND dotnet build --configuration Release
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/Slipper_Editor
    )
elseif(BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring Editor In Debug Config")
    add_custom_target(
        BuildEditor ALL
        COMMAND dotnet build --configuration Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/Slipper_Editor
    )
endif()


