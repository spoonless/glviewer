#include <iostream>
#include "gl.hpp"
#include "GLFW/glfw3.h"
#include "GlWindowContext.hpp"

static bool gladOk = false;

namespace glv
{
    namespace
    {

void errorCallback(int error, const char* description)
{
    std::cerr << "[" << error << "] " << description << std::endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

    }
}

void glv::GlWindowContext::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    GlWindowContext *glfw = static_cast<GlWindowContext*>(glfwGetWindowUserPointer(window));
    if(glfw)
    {
		glfw->_windowSize.x = static_cast<float>(width);
		glfw->_windowSize.y = static_cast<float>(height);
        glViewport(0, 0, width, height);
    }
}

glv::GlWindowContext::GlWindowContext() : _window(0)
{

}

glv::GlWindowContext::~GlWindowContext()
{
    glfwTerminate();
}

bool glv::GlWindowContext::init (std::string title, unsigned int width, unsigned int height)
{
    glfwSetErrorCallback(errorCallback);
    /* Initialize the library */
    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!_window) {
        return false;
    }
    glfwSetWindowTitle(_window, "GLSL viewer");

    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowSizeCallback(_window, windowSizeCallback);
    int realWidth,realHeight = 0;
    glfwGetWindowSize(_window, &realWidth, &realHeight);
	_windowSize.x = static_cast<float>(realWidth);
	_windowSize.y = static_cast<float>(realHeight);
    return true;
}

bool glv::GlWindowContext::makeCurrent()
{
    if (_window)
    {
        /* Make the window's context current */
        glfwMakeContextCurrent(_window);
        glfwSetKeyCallback(_window, keyCallback);
    }
    if (!gladOk)
    {
		gladOk = gladLoadGL() == GL_TRUE;
    }
    return gladOk;
}

bool glv::GlWindowContext::shouldContinue()
{
    return !glfwWindowShouldClose(_window);
}

void glv::GlWindowContext::swapAndPollEvents()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(_window);

    /* Poll for and process events */
    glfwPollEvents();
}

glm::vec2 glv::GlWindowContext::getCursorPosition()
{
    double x,y;
    glfwGetCursorPos(this->_window, &x, &y);
    return glm::vec2(static_cast<float>(x/_windowSize.x), static_cast<float>(1.0 - (y/_windowSize.y)));
}
