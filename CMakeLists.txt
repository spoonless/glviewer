cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(glviewer)

#########################################################################
# Project options
#########################################################################
option(BUILD_WITH_G3LOG "Use G3Log as logging system." ON)
option(BUILD_TESTING "Build all unit tests." ON)

#########################################################################
# Project metadata
#########################################################################

set(APP_NAME "GLViewer")
set(APP_AUTHOR "David Gayerie")
set(APP_VERSION "0.1.0")
string(TIMESTAMP NOW "%d/%m/%Y %H:%M")
set(APP_COMPILATION_DATE ${NOW})

#########################################################################
# Macros definitions
#########################################################################

macro(add_module MODULE_NAME)
    string(TOUPPER "${MODULE_NAME}" UPPER_MODULE_NAME)
    add_subdirectory("${MODULE_NAME}")
    set("${UPPER_MODULE_NAME}_INCLUDE_DIR" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/include")
    set("${UPPER_MODULE_NAME}_LIBRARY" "${MODULE_NAME}")
endmacro()

macro(config_executable PROJNAME)
    foreach(LIBNAME ${ARGN})
        include_directories( ${${LIBNAME}_INCLUDE_DIR} )
        target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
    endforeach()
endmacro()

macro(embed_package PROJ_NAME)
    include("Embed${PROJ_NAME}")
    string(TOUPPER "${PROJ_NAME}" UPPER_PROJ_NAME)
    include_directories(${${UPPER_PROJ_NAME}_INCLUDE_DIR})
endmacro()

#########################################################################
# Specific environment build configuration
#########################################################################

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -fno-rtti")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

if(MSVC)
    add_definitions(-DNOMINMAX)
endif()

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

include("SetCxxStandard")
set_cxx_standard(11)

if(BUILD_TESTING)
    enable_testing()
endif()

#########################################################################
# Embedding exernal projects
#########################################################################

embed_package(Glfw)
embed_package(Glm)
embed_package(Soil)
if(BUILD_WITH_G3LOG)
    embed_package(G3log)
    set(USE_G3LOG 1)
endif()
if(BUILD_TESTING)
    embed_package(Gtest)
endif()

#########################################################################
# building sub directories
#########################################################################

add_subdirectory(src)
