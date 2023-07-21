cmake_minimum_required(VERSION 3.16.0)

file(GLOB_RECURSE EDITOR_CONTENT *.*)
list(FILTER EDITOR_CONTENT EXCLUDE REGEX ".spv|CMakeLists.txt")

set(SLIPPER_EDITOR_CONTENT_NAME SlipperEditorContent CACHE STRING "Editor Content Name")

option(SLIPPER_COMPILE_SHADERS ON)
if(SLIPPER_COMPILE_SHADERS)
    message("Compiling Shaders")
    set(SHADER_DIR ${CMAKE_CURRENT_LIST_DIR}/Shaders)
    file(GLOB_RECURSE SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag ${SHADER_DIR}/*.comp ${SHADER_DIR}/*.geom ${SHADER_DIR}/*.tesc ${SHADER_DIR}/*.tese ${SHADER_DIR}/*.mesh ${SHADER_DIR}/*.task ${SHADER_DIR}/*.rgen ${SHADER_DIR}/*.rchit ${SHADER_DIR}/*.rmiss)
    find_package(Vulkan)

    foreach(SHADER IN LISTS SHADERS)
        #message("Shader: ${SHADER}")
        get_filename_component(FILENAME ${SHADER} NAME)
        set(COMPILED_SHADER_DIR ${SHADER_DIR}/Spir-V)
        add_custom_command(OUTPUT ${COMPILED_SHADER_DIR}/${FILENAME}.spv
            COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} --target-env vulkan1.2 -o ${COMPILED_SHADER_DIR}/${FILENAME}.spv ${SHADER}
            DEPENDS ${SHADER}
            COMMENT "$Compiling ${FILENAME}")
    list(APPEND SPV_SHADERS ${COMPILED_SHADER_DIR}/${FILENAME}.spv)
    endForeach()
endif()

list(APPEND EDITOR_CONTENT ${SPV_SHADERS})

#foreach(C ${EDITOR_CONTENT})
#    message("${C}")
#endForeach()

add_custom_target(${SLIPPER_EDITOR_CONTENT_NAME} ALL SOURCES ${EDITOR_CONTENT})

# Importet from SlipperEditor
GroupSources(../EditorContent EditorContent)

SetBuildDirectory()

set(OUTPUT_DIRECTORY ${BUILD_DIRECTORY}/$<CONFIG>)
message(STATUS "EditorContent Copy to: ${OUTPUT_DIRECTORY}")

add_custom_command(TARGET ${SLIPPER_EDITOR_CONTENT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${OUTPUT_DIRECTORY}/EditorContent/
                COMMENT "Copying from ${CMAKE_CURRENT_LIST_DIR} to ${OUTPUT_DIRECTORY}/EditorContent/")

add_custom_command(TARGET ${SLIPPER_EDITOR_CONTENT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E remove ${OUTPUT_DIRECTORY}/EditorContent/CMakeLists.txt)