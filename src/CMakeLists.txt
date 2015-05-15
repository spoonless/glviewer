
include_directories(
    "${CMAKE_BINARY_DIR}/include"
    "../src"
)

configure_file("config.h.in" "${CMAKE_BINARY_DIR}/include/config.h")

#########################################################################
# sub modules
#########################################################################

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

config_executable(core_sources GLAD SOIL SYS OGL)
target_link_libraries(core_sources ${CMAKE_THREAD_LIBS_INIT})

#########################################################################
# glslviewer
#########################################################################

add_executable(glslviewer
    main_glslviewer.cpp
)

set_target_properties(glslviewer
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(glslviewer core_sources)

add_custom_target(run
                  "$<TARGET_FILE:glslviewer>"
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_dependencies(run glslviewer)

#########################################################################
# simple loader
#########################################################################

add_executable(simpleloader
    main_simpleloader.cpp
)

set_target_properties(simpleloader
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(simpleloader core_sources)

#########################################################################
# unit tests
#########################################################################

if(BUILD_TESTING)
    add_executable(unittest
        ../tests/main.cpp
        ../tests/ObjModel_test.cpp
        ../tests/Camera_test.cpp
    )

    config_executable(unittest GTEST)
    target_link_libraries(unittest core_sources)

    add_test(unittest unittest)
    add_coverage(unittest unittest)
endif()
