#########################################################################
# Shared static library
#########################################################################

include_directories("include")

add_library(glviewer_lib STATIC
    include/GlMesh.hpp
    src/GlMesh.cpp
    include/ObjModel.hpp
    src/ObjModel.cpp
    include/Camera.hpp
    src/Camera.cpp
)

config_executable(glviewer_lib GLAD SOIL SYS OGL)
target_link_libraries(glviewer_lib ${CMAKE_THREAD_LIBS_INIT})

#########################################################################
# glviewer
#########################################################################

add_executable(glviewer
    src/main_glviewer.cpp
)

set_target_properties(glviewer
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(glviewer glviewer_lib)

add_custom_target(run
                  "$<TARGET_FILE:glviewer>"
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_dependencies(run glviewer)

#########################################################################
# OBJ info tool
#########################################################################

add_executable(objinfo
    src/main_objinfo.cpp
)

set_target_properties(objinfo
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(objinfo glviewer_lib)


#########################################################################
# module tests
#########################################################################

if(BUILD_TESTING)
    add_executable(test_glviewer
        tests/main.cpp
        tests/ObjModel_test.cpp
        tests/Camera_test.cpp
    )

    config_executable(test_glviewer GTEST)
    target_link_libraries(test_glviewer glviewer_lib)

    add_test(EXE_GLVIEWER test_glviewer)
    add_coverage(EXE_GLVIEWER test_glviewer)
endif()

