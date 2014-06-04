#include <iostream>
#include <cassert>
#include "gl.hpp"
#include <GLFW/glfw3.h>
#include "ShaderProgram.hpp"

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
        window = glfwCreateWindow(1280, 960, "Hello World", NULL, NULL);
        if (!window) {
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

    Tutorial1()
    {
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // An array of 3 vectors which represents 3 vertices
        static const GLfloat g_vertex_buffer_data[] = {
           -1.0f, -1.0f, 0.0f,
           1.0f, -1.0f, 0.0f,
           0.0f,  1.0f, 0.0f,
        };

        // This will identify our vertex buffer
        GLuint vertexbuffer;

        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);

        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void operator()()
    {
        glBindVertexArray(VertexArrayID);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

        glBindVertexArray(0);
    }

private:
    GLuint VertexArrayID;

};

class Tutorial2 : private Tutorial1
{
public:
    Tutorial2()
    {
        glv::Shader vs(glv::Shader::VERTEX_SHADER);
        vs.compile(
                    "#version 330 core\n"
                    "in vec3 vertices;\n"
                    "void main(){\n"
                    "  gl_Position = vec4(vertices, 1);\n"
                    "}\n"
        );

        glv::Shader fs(glv::Shader::FRAGMENT_SHADER);
        fs.compile(
                    "#version 330 core\n"
                    "out vec3 color;\n"
                    "void main(){\n"
                    "  color = vec3(1, 0, 0);\n"
                    "}\n"
        );

        program.attach(vs);
        program.attach(fs);
        program.link();
    }

    void operator()()
    {
        program.use();
        Tutorial1::operator ()();
    }

private:
    glv::ShaderProgram program;
};


timespec diff(timespec t1, timespec t2)
{
    timespec diff;
    if ((t2.tv_nsec > t1.tv_nsec)) {
        diff.tv_sec = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    } else {
        diff.tv_sec = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = 1000000000 - t2.tv_nsec + t1.tv_nsec;
    }
    return diff;
}

class Tutorial3
{
public:

    Tutorial3()
    {
        createVertexArray();
        createProgram();
    }

    void createVertexArray()
    {
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // An array of vectors which represents 3 vertices
        static const GLfloat g_vertex_buffer_data[] = {
           -1.0f, -1.0f, 0.0f,
           1.0f, -1.0f, 0.0f,
           -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };

        // This will identify our vertex buffer
        GLuint vertexbuffer;

        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);

        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void createProgram()
    {
        glv::Shader vs(glv::Shader::VERTEX_SHADER);
        vs.compile(
                    "#version 330 core\n"
                    "in vec3 vertices;\n"
                    "out vec2 surfacePosition;\n"
                    "void main(){\n"
                    "  gl_Position = vec4(vertices, 1);\n"
                    "  surfacePosition = vertices.xy;\n"
                    "}\n"
        );

        std::cout << vs.getLastCompilationLog() << std::endl;

        glv::Shader fs(glv::Shader::FRAGMENT_SHADER);
        fs.compile(
            "varying vec2 surfacePosition;\n"
            "uniform float time;\n"


            "const float color_intensity = .5;\n"
            "const float Pi = 3.14159;\n"

            "void main()\n"
            "{\n"
              "vec2 p=(1.32*surfacePosition);\n"
              "for(int i=1;i<5;i++)\n"
              "{\n"
                "vec2 newp=p;\n"
                "newp.x+=.912/float(i)*sin(float(i)*Pi*p.y+time*0.15)+0.91;\n"
                "newp.y+=.913/float(i)*cos(float(i)*Pi*p.x+time*-0.14)-0.91;\n"
                "p=newp;\n"
              "}\n"
              "vec3 col=vec3((sin(p.x+p.y)*.91+.1)*color_intensity);\n"
              "gl_FragColor=vec4(col, 1.0);\n"
            "}\n"
        );

        std::cout << fs.getLastCompilationLog() << std::endl;

        program.attach(vs);
        program.attach(fs);
        program.link();

        std::cout << program.getLastLinkLog() << std::endl;
    }

    void operator()()
    {
        program.use();

        timespec currenttime;
        clock_gettime(CLOCK_REALTIME, &currenttime);

        glProgramUniform1f(program.getId(), 0, (currenttime.tv_sec %1000) + ((float)currenttime.tv_nsec / 1000000000));

        glBindVertexArray(VertexArrayID);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle

        glBindVertexArray(0);
    }

private:
    GLuint VertexArrayID;
    glv::ShaderProgram program;
};

int main(int argc, char **argv)
{
    if (!glInit())
    {
        std::cerr<<"GL init failed, aborting."<< std::endl;
        return -1;
    }

    return tutoRunner<Tutorial3>(argc, argv);
}
