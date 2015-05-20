#include "log.hpp"
#include "gl.hpp"
#include "GLFW/glfw3.h"
#include "GlWindowContext.hpp"

static bool gladOk = false;

namespace ogl
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

void ogl::GlWindowContext::windowSizeCallback(GLFWwindow* window, int width, int height)
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

ogl::GlWindowContext::GlWindowContext() : _window{nullptr}
{

}

ogl::GlWindowContext::~GlWindowContext()
{
	if (_window)
	{
		LOG(INFO) << "Destroying window...";
		glfwDestroyWindow(_window);
		_window = nullptr;
	}
	LOG(INFO) << "Terminating GLFW...";
	glfwTerminate();
}

bool ogl::GlWindowContext::init (std::string title, unsigned int width, unsigned int height, bool fullscreenMode)
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

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* currentMode = glfwGetVideoMode(monitor);

    if (fullscreenMode)
    {
        width = width == 0u ? static_cast<unsigned int>(currentMode->width) : width;
        height = height == 0u ? static_cast<unsigned int>(currentMode->height) : height;
        LOGF(INFO, "Creating main window (%dx%d) in fullscreen mode...", width, height);
        _window = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);
        currentMode = glfwGetVideoMode(monitor);
    }
    else {
        width = width == 0u ? static_cast<unsigned int>(currentMode->width / 2) : width;
        height = height == 0u ? static_cast<unsigned int>(currentMode->height / 2) : height;
        LOGF(INFO, "Creating main window (%dx%d)...", width, height);
        _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    }

    if (!_window) {
        LOG(WARNING) << "fail to create window";
        return false;
    }

    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowSizeCallback(_window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
    int realWidth,realHeight = 0;
    glfwGetWindowSize(_window, &realWidth, &realHeight);
    windowSizeCallback(_window, realWidth, realHeight);

    return true;
}

bool ogl::GlWindowContext::makeCurrent()
{
    if (_window)
    {
        glfwMakeContextCurrent(_window);
        glfwSetKeyCallback(_window, keyCallback);
    }
    if (!gladOk)
    {
		if (gladLoadGL() != GL_TRUE)
		{
			LOG(WARNING) << "Cannot init GLAD!";
			return false;
		}
    }
    return true;
}

bool ogl::GlWindowContext::shouldContinue()
{
    return !glfwWindowShouldClose(_window);
}

void ogl::GlWindowContext::swapAndPollEvents()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

glm::vec2 ogl::GlWindowContext::getCursorPosition()
{
    double x,y;
    glfwGetCursorPos(this->_window, &x, &y);
    return glm::vec2(static_cast<float>(x/_windowSize.x), static_cast<float>(1.0 - (y/_windowSize.y)));
}

void ogl::GlWindowContext::setWindowSizeCallback(const std::function<void(unsigned int, unsigned int)>  &windowSizeCallback)
{
    _windowSizeCallback = windowSizeCallback;
    if (_window)
    {
        ogl::GlWindowContext::windowSizeCallback(_window, _windowSize.x, _windowSize.y);
    }
}
