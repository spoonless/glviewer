option(BUILD_WITH_G3LOG "Use G3Log as logging system." ON)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

embed_package(Glfw)
embed_package(Glm)
embed_package(Soil)
if(BUILD_TESTING)
    embed_package(Gtest)
endif()

###############################################
# main sources settings
###############################################

include_directories(
    "${GLM_INCLUDE_DIR}"
    "${CMAKE_BINARY_DIR}/include"
    "../src"
)

configure_file("config.h.in" "${CMAKE_BINARY_DIR}/include/config.h")

add_module(glad)
add_module(sys)
add_module(ogl)

add_library(core_sources STATIC
    GlMesh.hpp
    GlMesh.cpp
    ObjModel.hpp
    ObjModel.cpp
    Camera.hpp
    Camera.cpp
)

add_dependencies(core_sources glm)
config_executable(core_sources GLAD SYS OGL GLFW SOIL OPENGL)

target_link_libraries(core_sources ${CMAKE_THREAD_LIBS_INIT})

###############################################
# glslviewer executable settings
###############################################
add_executable(glslviewer
    main_glslviewer.cpp
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
    main_simpleloader.cpp
)
target_link_libraries(simpleloader core_sources)

###############################################
# unit tests project settings
###############################################
#    enable_testing()
#    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
#        include("CodeCoverage")
#        setup_target_for_coverage(coverage unittest coverage)
#    endif()

if(BUILD_TESTING)
    add_executable(unittest
        ../tests/main.cpp
        ../tests/ObjModel_test.cpp
        ../tests/Camera_test.cpp
    )

    config_executable(unittest GTEST)
    target_link_libraries(unittest core_sources)

    add_test(unittest unittest)
endif()
