include_directories("include")

add_library(ogl STATIC
    include/gl.hpp
    src/gl.cpp
    include/GlError.hpp
    src/GlError.cpp
    include/Shader.hpp
    src/Shader.cpp
    include/ShaderProgram.hpp
    src/ShaderProgram.cpp
    include/UniformDeclaration.hpp
    src/UniformDeclaration.cpp
    include/GlWindowContext.hpp
    src/GlWindowContext.cpp
    include/Duration.hpp
    src/Duration.cpp
)

config_executable(ogl GLAD SYS GLFW OPENGL)

if(BUILD_TESTING)
    add_executable(ogl_test
        tests/main.cpp
        tests/gl_test.cpp
        tests/GlError_test.cpp
        tests/Shader_test.cpp
        tests/ShaderProgram_test.cpp
        tests/UniformDeclaration_test.cpp
    )

    config_executable(ogl_test GTEST)
    target_link_libraries(ogl_test ogl)

    add_test(MODULE_OGL ogl_test)
endif()
