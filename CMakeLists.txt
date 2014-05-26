cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(wavefront)

option(BUILD_UNITTESTS "Build all unit tests." "1")

if(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

macro( config_executable PROJNAME )
    foreach(LIBNAME ${ARGN})
        include_directories( ${${LIBNAME}_INCLUDE_DIR} )
        target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
    endforeach()
endmacro()

###############################################
# main sources settings
###############################################
# necessary for mingw32
include_directories("src")

add_library(core_sources STATIC
    src/ObjModel.hpp
    src/ObjModel.cpp
)

###############################################
# main executable settings
###############################################
add_executable(main
    src/main.cpp
)
target_link_libraries(main core_sources)

add_custom_target(run
                  "$<TARGET_FILE:main>"
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_dependencies(run main)

###############################################
# unit tests project settings
###############################################
if(BUILD_UNITTESTS)
    include("EmbedGtest")
    include_directories("src")
    add_executable(unittest
                   tests/main.cpp
                   tests/ObjModel_test.cpp
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
