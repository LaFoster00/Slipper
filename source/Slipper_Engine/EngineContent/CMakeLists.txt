cmake_minimum_required(VERSION 3.9.0)

file(GLOB_RECURSE ENGINE_CONTENT *.*)
list(FILTER ENGINE_CONTENT EXCLUDE REGEX ".spv|CMakeLists.txt")

option(COMPILE_SHADERS ON)
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
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${COMPILED_SHADER_DIR}/${FILENAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Compiling ${FILENAME}")
    list(APPEND SPV_SHADERS ${COMPILED_SHADER_DIR}/${FILENAME}.spv)
    endForeach()
endif()

list(APPEND ENGINE_CONTENT ${SPV_SHADERS})

#foreach(C ${ENGINE_CONTENT})
#    message("${C}")
#endForeach()

add_custom_target(SlipperEngine_EngineContent ALL SOURCES ${ENGINE_CONTENT})

# Importet from SlipperEngine
GroupSources(..\\EngineContent ..\\EngineContent EngineContent)