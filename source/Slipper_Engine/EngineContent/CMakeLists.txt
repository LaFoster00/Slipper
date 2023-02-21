cmake_minimum_required(VERSION 3.16.0)

file(GLOB_RECURSE ENGINE_CONTENT *.*)
list(FILTER ENGINE_CONTENT EXCLUDE REGEX ".spv|CMakeLists.txt")

option(SLIPPER_COMPILE_SHADERS ON)
if(COMPILE_SHADERS)
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

list(APPEND ENGINE_CONTENT ${SPV_SHADERS})

#foreach(C ${ENGINE_CONTENT})
#    message("${C}")
#endForeach()

add_custom_target(SlipperEngine_EngineContent ALL SOURCES ${ENGINE_CONTENT})

# Importet from SlipperEngine
GroupSources(..\\EngineContent EngineContent)

if(NOT DEFINED OUTPUT_DIRECTORY)
# Use $^<CONFIG^> instead of $<CONFIG> since the later one simply doesnt work when the only this target is build
set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin")
endif()

set(OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/$<CONFIG>)
message(STATUS ${OUTPUT_DIRECTORY})

add_custom_command(TARGET SlipperEngine_EngineContent POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${OUTPUT_DIRECTORY}/EngineContent/
                COMMENT "Copying from ${CMAKE_CURRENT_LIST_DIR} to ${OUTPUT_DIRECTORY}/EngineContent/")

add_custom_command(TARGET SlipperEngine_EngineContent POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E remove ${OUTPUT_DIRECTORY}/EngineContent/CMakeLists.txt)