#include "log.hpp"
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
    LOG(WARNING) << "[" << error << "] " << description;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

    }
}

void glv::GlWindowContext::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    GlWindowContext *glfw = static_cast<GlWindowContext*>(glfwGetWindowUserPointer(window));
    if(glfw)
    {
        glfw->_windowSize = {width, height};
        if (glfw->_windowSizeCallback)
        {
            glfw->_windowSizeCallback(width, height);
        }
    }
}

glv::GlWindowContext::GlWindowContext() : _window{nullptr}
{

}

glv::GlWindowContext::~GlWindowContext()
{
    glfwTerminate();
}

bool glv::GlWindowContext::init (std::string title, unsigned int width, unsigned int height)
{
    glfwSetErrorCallback(errorCallback);
    LOG(INFO) << "Initializing GLFW...";
    if (!glfwInit())
    {
        LOG(WARNING) << "fail to initialize GLFW!";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    LOGF(INFO, "Creating main window (%dx%d)...", width, height);
    _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!_window) {
        LOG(WARNING) << "fail to create window";
        return false;
    }
    glfwSetWindowTitle(_window, "GLSL viewer");

    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowSizeCallback(_window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
    int realWidth,realHeight = 0;
    glfwGetWindowSize(_window, &realWidth, &realHeight);
    windowSizeCallback(_window, width, height);

    return true;
}

bool glv::GlWindowContext::makeCurrent()
{
    if (_window)
    {
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
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

glm::vec2 glv::GlWindowContext::getCursorPosition()
{
    double x,y;
    glfwGetCursorPos(this->_window, &x, &y);
    return glm::vec2(static_cast<float>(x/_windowSize.x), static_cast<float>(1.0 - (y/_windowSize.y)));
}

void glv::GlWindowContext::setWindowSizeCallback(const std::function<void(unsigned int, unsigned int)>  &windowSizeCallback)
{
    _windowSizeCallback = windowSizeCallback;
    if (_window)
    {
        glv::GlWindowContext::windowSizeCallback(_window, _windowSize.x, _windowSize.y);
    }
}
