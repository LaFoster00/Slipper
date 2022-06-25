cmake_minimum_required(VERSION 3.9.0)
project(Slipper VERSION 0.1.0)

include(CTest)
enable_testing()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE CACHE BOOL "Is Linux Platform")
elseif(NOT APPLE)
    set(LINUX FALSE CACHE BOOL "Not Linux Platform")
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


#GLM
message(STATUS "Configuring GLM")
add_subdirectory(extern/glm)


#GLFW
message(STATUS "Configuring GLFW")
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(extern/glfw ${CMAKE_CURRENT_LIST_DIR}/extern/glfw/build)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()


#Slipper
message(STATUS "Configuring SLIPPER_ENGINE Lib")
set(OUTPUT_LIB OFF CACHE INTERNAL "")
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


