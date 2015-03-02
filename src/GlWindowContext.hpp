#ifndef GL_WINDOW_CONTEXT_HPP
#define GL_WINDOW_CONTEXT_HPP

#include "glm/vec2.hpp"

struct GLFWwindow;

namespace glv
{

class GlWindowContext
{
public:

    GlWindowContext();
    ~GlWindowContext();

    GlWindowContext(const GlWindowContext &) = delete;
    GlWindowContext & operator = (const GlWindowContext &) = delete;

    bool init(std::string title, unsigned int width, unsigned int height);

    bool makeCurrent();

    bool shouldContinue();

    void swapAndPollEvents();

    glm::vec2 getCursorPosition();

    inline glm::vec2 getWindowSize()
    {
        return _windowSize;
    }

private:
    static void windowSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow *_window;
    glm::vec2 _windowSize;
};

}

#endif
