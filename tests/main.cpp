#include <iostream>
#include "gtest/gtest.h"
#include "gl.hpp"
#include "GLFW/glfw3.h"

namespace {

class GlContext
{
public:
    GlContext() : window(0)
    {
    }

    ~GlContext()
    {
        glfwDestroyWindow(window);
    }

    bool makeCurrent()
    {
        if (!gl3wInit())
        {
            std::cerr << "Cannot initialise GL3W" << std::endl;
            return false;
        }

        if(!glfwInit())
        {
            std::cerr << "Cannot initialise GLFW" << std::endl;
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(10, 10, "Test", NULL, NULL);
        glfwMakeContextCurrent(window);
        return true;
    }

private:
    GLFWwindow *window;
};

}

int main(int argc, char **argv) {
    GlContext glContext;
    if(! glContext.makeCurrent())
    {
        return 1;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
