#########################################################################
# Inspired (but unless entirely rewritten) from Lars Bilke script
# See original @ https://github.com/bilke/cmake-modules
#
# To enable code coverage, copy this script in a directory available
# in CMAKE_MODULE_PATH.
#
# include("CodeCoverage")
#
# enable_coverage()
#
# add_coverage(NAME EXE)
#
#########################################################################

if(NOT CMAKE_COMPILER_IS_GNUCXX OR NOT(CMAKE_BUILD_TYPE STREQUAL "Coverage"))
    macro(enable_coverage)
        if(NOT CMAKE_COMPILER_IS_GNUCXX)
            message(FATAL_ERROR "Code coverage is, until now, only available with GNU c++ compatible compiler")
        endif()
    endmacro()

    macro(add_coverage NAME EXE)
    endmacro()

    return()
endif()

find_program(GCOV_EXE gcov)
find_program(LCOV_EXE lcov)
find_program(GENHTML_EXE genhtml)

if(NOT GCOV_EXE)
    message(FATAL_ERROR "gcov not found! Aborting...")
endif()

if(NOT LCOV_EXE)
    message(FATAL_ERROR "lcov not found! Aborting...")
endif()

if(NOT GENHTML_EXE)
    message(FATAL_ERROR "genhtml not found! Aborting...")
endif()

set(CMAKE_CXX_FLAGS_COVERAGE
    "-g -O0 --coverage -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE )

set(CMAKE_C_FLAGS_COVERAGE
    "-g -O0 --coverage -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE )

set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE )

set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE )

mark_as_advanced(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE )


macro(enable_coverage)
    add_custom_target(coverage)
    set_directory_properties(${CMAKE_BINARY_DIR} ADDITIONAL_MAKE_CLEAN_FILES coverage)
    add_custom_command(
        TARGET coverage
        POST_BUILD
        COMMAND ${GENHTML_EXE} -o coverage *.info
        COMMAND ${CMAKE_COMMAND} -E remove *.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_command(
        TARGET coverage
        POST_BUILD
        COMMAND ;
        COMMENT "Open ./coverage/index.html in your browser to view the coverage report."
    )
endmacro()

macro(add_coverage NAME EXE)
    add_custom_target(coverage_${NAME}
        # Cleanup lcov
        ${LCOV_EXE} --directory . --zerocounters
        # Run tests
        COMMAND ${EXE} ${ARGV3}
        # Capturing lcov counters and generating report
        COMMAND ${LCOV_EXE} --directory . --capture --output-file ${NAME}.tmp.info
        COMMAND ${LCOV_EXE} --remove ${NAME}.tmp.info 'tests/*' 'gtest/*' '/usr/*' --output-file ${NAME}.info -q
        COMMAND ${CMAKE_COMMAND} -E remove ${NAME}.tmp.info

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report for ${NAME}."
    )

    add_dependencies(coverage coverage_${NAME})
endmacro()
