cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(glviewer)

set(APP_NAME "GLViewer")
set(APP_AUTHOR "David Gayerie")
set(APP_VERSION "0.1.0")
string(TIMESTAMP NOW "%d/%m/%Y %H:%M")
set(APP_COMPILATION_DATE ${NOW})

option(BUILD_UNITTESTS "Build all unit tests." ON)
option(BUILD_WITH_G3LOG "Use G3Log as logging system." ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

if(MSVC)
    add_definitions(-DNOMINMAX)
endif()

macro( config_executable PROJNAME )
    foreach(LIBNAME ${ARGN})
        include_directories( ${${LIBNAME}_INCLUDE_DIR} )
        target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
    endforeach()
endmacro()

include("SetCxxStandard")
set_cxx_standard(11)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

include("EmbedGlfw")
include("EmbedGlm")
include("EmbedSoil")
if(BUILD_WITH_G3LOG)
    include("EmbedG3log")
    set(USE_G3LOG 1)
endif()

###############################################
# main sources settings
###############################################

include_directories(
    "${GLM_INCLUDE_DIR}"
    "${CMAKE_BINARY_DIR}/include"
    "include"
    "src"
)

configure_file("src/config.h.in" "${CMAKE_BINARY_DIR}/include/config.h")

add_library(core_sources STATIC
    src/gl.hpp
    src/gl.cpp
    src/glad.c
    src/log.hpp
    src/log.cpp
    src/Path.hpp
    src/Path.cpp
    src/Duration.hpp
    src/Duration.cpp
    src/GlError.hpp
    src/GlError.cpp
    src/GlWindowContext.hpp
    src/GlWindowContext.cpp
    src/GlMesh.hpp
    src/GlMesh.cpp
    src/ObjModel.hpp
    src/ObjModel.cpp
    src/OperationResult.hpp
    src/Shader.hpp
    src/Shader.cpp
    src/ShaderProgram.hpp
    src/ShaderProgram.cpp
    src/UniformDeclaration.hpp
    src/UniformDeclaration.cpp
    src/Camera.hpp
    src/Camera.cpp
)

add_dependencies(core_sources glm)
config_executable(core_sources glfw GLFW SOIL OPENGL)
if(BUILD_WITH_G3LOG)
    config_executable(core_sources G3LOG)
endif()
target_link_libraries(core_sources ${CMAKE_THREAD_LIBS_INIT})

###############################################
# glslviewer executable settings
###############################################
add_executable(glslviewer
    src/main_glslviewer.cpp
)
target_link_libraries(glslviewer core_sources)

add_custom_target(run
                  "$<TARGET_FILE:glslviewer>"
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_dependencies(run glslviewer)

###############################################
# simple loader executable settings
###############################################
add_executable(simpleloader
    src/main_simpleloader.cpp
)
target_link_libraries(simpleloader core_sources)

###############################################
# unit tests project settings
###############################################
if(BUILD_UNITTESTS)
    include("EmbedGtest")
    add_executable(unittest
        tests/main.cpp
        tests/Path_test.cpp
        tests/gl_test.cpp
        tests/GlError_test.cpp
        tests/ObjModel_test.cpp
        tests/Shader_test.cpp
        tests/ShaderProgram_test.cpp
        tests/UniformDeclaration_test.cpp
        tests/Camera_test.cpp
    )

    config_executable(unittest GTEST)
    target_link_libraries(unittest core_sources)

    add_custom_target(test
                      "$<TARGET_FILE:unittest>"
                      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    add_dependencies(test unittest)

    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
        include("CodeCoverage")
        setup_target_for_coverage(coverage unittest coverage)
    endif()
endif()
