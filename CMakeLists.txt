set(BUILD_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../SlipperBin/bin")


macro(SetBuildDirectory)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIRECTORY})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIRECTORY})
    set(CMAKE_BINARY_DIR ${BUILD_DIRECTORY})
    set(CMAKE_CURRENT_BINARY_DIR ${BUILD_DIRECTORY})
endmacro()

SetBuildDirectory()

message(STATUS "Cmake Source dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "Cmake Binary dir: ${CMAKE_BINARY_DIR}")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  if(NOT DEFINED WITH_IN_SOURCE_BUILD)
    message(FATAL_ERROR
      "CMake generation for Slipper is not allowed within the source directory!"
      "\n Remove \"${CMAKE_SOURCE_DIR}/CMakeCache.txt\" and try again from another folder, e.g.:"
      "\n "
      "\n rm -rf CMakeCache.txt CMakeFiles"
      "\n cd .."
      "\n mkdir cmake-make"
      "\n cd cmake-make"
      "\n cmake ../blender"
      "\n "
      "\n Alternately define WITH_IN_SOURCE_BUILD to force this option (not recommended!)"
    )
  endif()
endif()

cmake_minimum_required(VERSION 3.16.0)
project(Slipper VERSION 0.1.0)

include(CTest)
enable_testing()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
    add_compile_definitions(LINUX)
elseif(NOT APPLE)
    set(LINUX FALSE)
    add_compile_definitions(WINDOWS)
endif()

set(EXTERNAL_LIBS_DIR "${CMAKE_CURRENT_LIST_DIR}/extern")

string(CONCAT BUILD_TYPE ${CMAKE_BUILD_TYPE})

find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/.git")
#Udate submodule as needed
    option(SLIPPER_GIT_SUBMODULE_UPDATE "Check submodules during build" ON)
    if(SLIPPER_GIT_SUBMODULE_UPDATE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/extern/glfw/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# Special thanks to Luca Gherardi who wrote this special little peace of code :)
# Only used for VS since it does not sort source files based on their directory otherwise
function(GroupSources_Internal curdir originaldir rootsolutiondir)
    file(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR}/${curdir} ${CMAKE_CURRENT_LIST_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
            GroupSources_Internal(${curdir}/${child} ${originaldir} ${rootsolutiondir})
        else()
            if(NOT ${child} STREQUAL CMakeLists.txt)
                string(REPLACE "${originaldir}" "${rootsolutiondir}" groupname ${curdir})
                #message("Child dir ${groupname}")
                #message("Child entry ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child}")
                #message("Group entry ${groupname}/${child}")
                source_group(${groupname} FILES ${CMAKE_CURRENT_LIST_DIR}/${curdir}/${child})
            endif()
        endif()
    endforeach()
endfunction()

function(GroupSources curdir rootsolutiondir)
    GroupSources_Internal(${curdir} ${curdir} ${rootsolutiondir}) 
endfunction()


#Vulkan
find_package(Vulkan)
if(NOT ${Vulkan_FOUND})
    message( FATAL_ERROR "Vulkan was not found!")
endif()

#GLM
message(STATUS "Configuring GLM")
add_subdirectory(extern/glm)

#GLFW
message(STATUS "Configuring GLFW")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${EXTERNAL_LIBS_DIR}/glfw)

#imgui and ImGuizmo
message(STATUS "Configuring imgui")
set(IMGUI_FILES
    ${EXTERNAL_LIBS_DIR}/imgui/imgui.h 
    ${EXTERNAL_LIBS_DIR}/imgui/imgui.cpp 
    ${EXTERNAL_LIBS_DIR}/imgui/imgui_demo.cpp
    ${EXTERNAL_LIBS_DIR}/imgui/imgui_draw.cpp 
    ${EXTERNAL_LIBS_DIR}/imgui/imgui_tables.cpp 
    ${EXTERNAL_LIBS_DIR}/imgui/imgui_widgets.cpp
    ${EXTERNAL_LIBS_DIR}/imgui/backends/imgui_impl_glfw.h
    ${EXTERNAL_LIBS_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${EXTERNAL_LIBS_DIR}/imgui/backends/imgui_impl_vulkan.h
    ${EXTERNAL_LIBS_DIR}/imgui/backends/imgui_impl_vulkan.cpp

    #ImGuizmo
    ${EXTERNAL_LIBS_DIR}/ImGuizmo/ImGuizmo.h
    ${EXTERNAL_LIBS_DIR}/ImGuizmo/ImGuizmo.cpp)

add_library(IMGUI STATIC ${IMGUI_FILES})
# private link and include dependencies so that users of this library can use their own linking
target_link_libraries(IMGUI PRIVATE ${Vulkan_LIBRARIES} glfw)
target_include_directories(IMGUI PRIVATE ${Vulkan_INCLUDE_DIRS})
# public include for convinient usage later on
target_include_directories(IMGUI PUBLIC
    ${EXTERNAL_LIBS_DIR}/imgui
    ${EXTERNAL_LIBS_DIR}/ImGuizmo
    )
#group sources in project overview
GroupSources(extern/imgui Source/Iimgui)
GroupSources(extern/ImGuizmo Source/ImGuizmo)


if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

#Slipper Engine
message(STATUS "Configuring SLIPPER_ENGINE Lib")
add_subdirectory(source/Slipper_Engine)

#Slipper Editor
message(STATUS "Configuring SLIPPER_ENGINE")
add_subdirectory(source/Slipper_Editor)