cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(glviewer)

option(BUILD_TESTING "Build all unit tests." ON)

set(APP_NAME "GLViewer")
set(APP_AUTHOR "David Gayerie")
set(APP_VERSION "0.1.0")
string(TIMESTAMP NOW "%d/%m/%Y %H:%M")
set(APP_COMPILATION_DATE ${NOW})

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

if(MSVC)
    add_definitions(-DNOMINMAX)
endif()

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
endmacro()

include("SetCxxStandard")
set_cxx_standard(11)

if(BUILD_TESTING)
    enable_testing()
endif()


add_subdirectory(src)
