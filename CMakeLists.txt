cmake_minimum_required(VERSION 3.9.0)
project(Slipper VERSION 0.1.0)

include(CTest)
enable_testing()

if(UNIX AND NOT APPLE)
        set(LINUX TRUE)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

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


option(LIB_DEBUG OFF)

execute_process(
    COMMAND make
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/extern/bgfx
)

if (LINUX)
    if(LIB_DEBUG)
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
    if(LIB_DEBUG)
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
message(STATUS "BUILDING GLFW")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw ${CMAKE_CURRENT_LIST_DIR}/extern/glfw/build)


#Slipper
message(STATUS "BUILDING SLIPPER_ENGINE")
add_subdirectory(source/Slipper_Engine)

message(STATUS "BUILDING SLIPPER_EDITOR")
if (BUILD_TYPE STREQUAL "release")
    message(STATUS "Building Editor In Release Config")    
    execute_process(
        COMMAND dotnet build --configuration Release
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/Slipper_Editor
    )
elseif(BUILD_TYPE STREQUAL "debug")
    message(STATUS "Building Editor In Debug Config")
    execute_process(
        COMMAND dotnet build --configuration Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/Slipper_Editor
    )
endif()
