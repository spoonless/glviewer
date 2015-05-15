#########################################################################
# module library
#########################################################################

include_directories("include")

add_library(sys STATIC
    include/OperationResult.hpp
    include/Duration.hpp
    src/Duration.cpp
    include/CommandLineParser.hpp
    src/CommandLineParser.cpp
    include/log.hpp
    src/log.cpp
    include/Path.hpp
    src/Path.cpp
    include/LineReader.hpp
    src/LineReader.cpp
)

config_executable(sys G3LOG)
target_link_libraries(sys ${CMAKE_THREAD_LIBS_INIT})

#########################################################################
# module tests
#########################################################################

if(BUILD_TESTING)
    add_executable(test_sys
        tests/main.cpp
        tests/Path_test.cpp
        tests/CommandLineParser_test.cpp
        tests/LineReader_test.cpp
    )

    config_executable(test_sys GTEST)
    target_link_libraries(test_sys sys)

    add_test(MODULE_SYS test_sys)
    add_coverage(MODULE_SYS test_sys)
endif()
