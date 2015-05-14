if(BUILD_WITH_G3LOG)
    embed_package(G3log)
    set(USE_G3LOG 1)
endif()

include_directories("include")

add_library(sys STATIC
    "include/OperationResult.hpp"
    "include/CommandLineParser.hpp"
    "src/CommandLineParser.cpp"
    "include/log.hpp"
    "src/log.cpp"
    "include/Path.hpp"
    "src/Path.cpp"
)

config_executable(sys G3LOG)
target_link_libraries(sys ${CMAKE_THREAD_LIBS_INIT})

if(BUILD_TESTING)
    add_executable(sys_test
        tests/main.cpp
        tests/Path_test.cpp
        tests/CommandLineParser_test.cpp
    )

    config_executable(sys_test GTEST)
    target_link_libraries(sys_test sys)

    add_test(MODULE_SYS sys_test)
endif()
