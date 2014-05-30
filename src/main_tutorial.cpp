#include <cassert>
#include <GL/gl3w.h>
#define GLFW_INCLUDE_GL_3
#include <GLFW/glfw3.h>
#include <iostream>

bool glInit()
{
    if(gl3wInit())
    {
        return true;
    }
    return false;
}

class glframework
{
public:
    static void error_callback(int error, const char* description)
    {
        std::cerr << "[" << error << "] " << description << std::endl;
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glframework() : window(0)
    {

    }

    ~glframework()
    {
        glfwTerminate();
    }

    bool init ()
    {
        glfwSetErrorCallback(error_callback);
        /* Initialize the library */
        if (!glfwInit())
        {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        return true;
    }

    void makeCurrent()
    {
        if (window)
        {
            /* Make the window's context current */
            glfwMakeContextCurrent(window);
            glfwSetKeyCallback(window, key_callback);
        }
    }

    bool shouldContinue()
    {
        return !glfwWindowShouldClose(window);
    }

    void swapAndPollEvents()
    {
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

private:
    GLFWwindow *window;
};

template<typename Tuto>
int tutoRunner(int argc, char **argv)
{
    glframework glfw;
    if (!glfw.init())
    {
        std::cerr<<"GLFW failed, aborting."<< std::endl;
        return -1;
    }

    glfw.makeCurrent();

    Tuto tuto;

    /* Loop until the user closes the window */
    while (glfw.shouldContinue())
    {
        tuto();
        glfw.swapAndPollEvents();
    }
    return 0;
}

class Tutorial1
{
public:
    void operator()()
    {
        static float vertices[] = {-0.5, -0.5, 0.0, 0.5, 0.5, -0.5};

        glClear(GL_COLOR_BUFFER_BIT);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(0);

    }
};


class Tutorial2
{
public:

    Tutorial2()
    {
        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // An array of 3 vectors which represents 3 vertices
        static const GLfloat g_vertex_buffer_data[] = {
           -1.0f, -1.0f, 0.0f,
           1.0f, -1.0f, 0.0f,
           0.0f,  1.0f, 0.0f,
        };

        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);

        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    }

    void operator()()
    {
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

        glDisableVertexAttribArray(0);
    }

private:
    // This will identify our vertex buffer
    GLuint vertexbuffer;

};

int main(int argc, char **argv)
{
    if (!glInit())
    {
        std::cerr<<"GL init failed, aborting."<< std::endl;
        return -1;
    }

    return tutoRunner<Tutorial1>(argc, argv);
}
