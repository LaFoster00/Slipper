cmake_minimum_required(VERSION 3.16.0)
project(Slipper VERSION 0.1.0)

include(CTest)
enable_testing()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE CACHE BOOL "Is Linux Platform")
    add_compile_definitions(LINUX)
elseif(NOT APPLE)
    set(LINUX FALSE CACHE BOOL "Not Linux Platform")
    add_compile_definitions(WINDOWS)
endif()

set(EXTERNAL_LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/extern")

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

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/extern/glfw/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# Special thanks to Luca Gherardi who wrote this special little peace of code :)
function(GroupSources curdir rootfiledir rootsolutiondir)
    file(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR}/${curdir} ${CMAKE_CURRENT_LIST_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
            GroupSources(${curdir}/${child} ${rootfiledir} ${rootsolutiondir})
        else()
            if(NOT ${child} STREQUAL CMakeLists.txt)
                string(REPLACE "/" "\\" groupname ${curdir})
                #message("Child dir ${groupname}")
                #message("Child entry ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child}")
                # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)
                string(REPLACE "${rootfiledir}" "${rootsolutiondir}" groupname ${groupname})
                #message("Groupname entry ${groupname}")
                source_group(${groupname} FILES ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
            endif()
        endif()
    endforeach()
endfunction()

#GLM
message(STATUS "Configuring GLM")
add_subdirectory(extern/glm)

#GLFW
message(STATUS "Configuring GLFW")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${EXTERNAL_LIBS_DIR}/glfw)

#imgui
message(STATUS "Configuring imgui")
set(IMGUI_FILES ${EXTERNAL_LIBS_DIR}/imgui/imgui.cpp ${EXTERNAL_LIBS_DIR}/imgui/imgui_demo.cpp ${EXTERNAL_LIBS_DIR}/imgui/imgui_draw.cpp ${EXTERNAL_LIBS_DIR}/imgui/imgui_tables.cpp ${EXTERNAL_LIBS_DIR}/imgui/imgui_widgets.cpp ${EXTERNAL_LIBS_DIR}/imgui/backends/imgui_impl_glfw.cpp ${EXTERNAL_LIBS_DIR}/imgui/backends/imgui_impl_vulkan.cpp)
set(IMGUIZMO_FILES ${EXTERNAL_LIBS_DIR}/ImGuizmo/ImGuizmo.cpp)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()


#Slipper Engine
message(STATUS "Configuring SLIPPER_ENGINE Lib")
set(PRINTF_IN_SHADERS OFF CACHE BOOL "")
add_subdirectory(source/Slipper_Engine)

#Slipper Editor
message(STATUS "Configuring SLIPPER_ENGINE")
add_subdirectory(source/Slipper_Editor)

# option(BUILD_EDITOR OFF)
# if(BUILD_EDITOR)
#     message(STATUS "Configuring SLIPPER_EDITOR")
#     if (BUILD_TYPE STREQUAL "Release")
#         message(STATUS "Configuring Editor In Release Config")
#         add_custom_target(
#                 BuildEditor ALL
#                 COMMAND dotnet build --configuration Release
#                 WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/Slipper_Editor
#         )
#     elseif(BUILD_TYPE STREQUAL "Debug")
#         message(STATUS "Configuring Editor In Debug Config")
#         add_custom_target(
#                 BuildEditor ALL
#                 COMMAND dotnet build --configuration Debug
#                 WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/source/Slipper_Editor
#         )
#     endif()
# endif()

