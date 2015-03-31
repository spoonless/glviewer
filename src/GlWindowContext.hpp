#ifndef GL_WINDOW_CONTEXT_HPP
#define GL_WINDOW_CONTEXT_HPP

#include <functional>
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

    void setWindowSizeCallback(const std::function<void(unsigned int, unsigned int)>  &windowSizeCallback);

private:
    static void windowSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow *_window;
    glm::uvec2 _windowSize;
    std::function<void(unsigned int, unsigned int)> _windowSizeCallback;
};

}

#endif
