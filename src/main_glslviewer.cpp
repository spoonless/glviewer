#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include "gl.hpp"
#include "glm/vec2.hpp"
#include "GLFW/glfw3.h"
#include "ShaderProgram.hpp"

class glframework
{
public:
    static void errorCallback(int error, const char* description)
    {
        std::cerr << "[" << error << "] " << description << std::endl;
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    static void windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        glframework *glfw = static_cast<glframework*>(glfwGetWindowUserPointer(window));
        if(glfw)
        {
            glfw->windowSize.x = width;
            glfw->windowSize.y = height;
            glViewport(0, 0, width, height);
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
        glfwSetErrorCallback(errorCallback);
        /* Initialize the library */
        if (!glfwInit())
        {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(800, 600, "Viewer", NULL, NULL);
        if (!window) {
            return false;
        }

        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, windowSizeCallback);
        int width,height = 0;
        glfwGetWindowSize(window, &width, &height);
        windowSize.x = width;
        windowSize.y = height;
        return true;
    }

    void makeCurrent()
    {
        if (window)
        {
            /* Make the window's context current */
            glfwMakeContextCurrent(window);
            glfwSetKeyCallback(window, keyCallback);
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

    glm::fvec2 getCursorPosition()
    {
        double x,y;
        glfwGetCursorPos(this->window, &x, &y);
        return glm::vec2(static_cast<float>(x/windowSize.x), static_cast<float>(1.0 - (y/windowSize.y)));
    }

    glm::vec2 getWindowSize()
    {
        return windowSize;
    }

    void setTitle(const std::string& title)
    {
        if (window)
        {
            std::string completeTitle = "GLSL Viewer - ";
            completeTitle += title;
            glfwSetWindowTitle(window, completeTitle.c_str());
        }
    }


private:
    GLFWwindow *window;
    glm::vec2 windowSize;
};

const char defaultFragmentShader[] =
        GLSL_VERSION_HEADER
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
        "}\n";

class GlslViewer
{
public:

    GlslViewer(int argc, char **argv) : vertexArrayID(0)
    {
        std::string fragmentShader;
        if (argc > 1)
        {
            std::ifstream shaderFile(argv[1]);
            std::getline(shaderFile, fragmentShader, '\0');
            if (!fragmentShader.empty())
            {
                fragmentShader.insert(0, GLSL_VERSION_HEADER);
            }
            title = std::string("/") + argv[1];
            title = title.substr(title.find_last_of("\\/") + 1);
        }
        if (fragmentShader.empty())
        {
            title = "default";
            fragmentShader = defaultFragmentShader;
        }
        createVertexArray();
        createProgram(fragmentShader);
    }

    ~GlslViewer()
    {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vertexArrayID);
    }

    void createVertexArray()
    {
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

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
        //glDeleteBuffers(1, &vertexbuffer);
    }

    void createProgram(const std::string &fragmentShader)
    {
        glv::Shader vs(glv::Shader::VERTEX_SHADER);
        vs.compile(
                    GLSL_VERSION_HEADER
                    "in vec3 vertices;\n"
                    "out vec2 surfacePosition;\n"
                    "void main(){\n"
                    "  gl_Position = vec4(vertices, 1);\n"
                    "  surfacePosition = vertices.xy;\n"
                    "}\n"
        );

        std::cout << vs.getLastCompilationLog() << std::endl;

        glv::Shader fs(glv::Shader::FRAGMENT_SHADER);
        if(! fs.compile(fragmentShader))
        {
            std::cout << "Cannot compile fragment shader!" << std::endl;
        }

        std::cout << fs.getLastCompilationLog() << std::endl;

        program.attach(vs);
        program.attach(fs);
        if(!program.link())
        {
            std::cout << "Cannot link shader program!" << std::endl;
        }

        std::cout << program.getLastLinkLog() << std::endl;

        program.use();

        timeUniform = program.getActiveUniform("time");
        mouseUniform = program.getActiveUniform("mouse");
        resolutionUniform = program.getActiveUniform("resolution");
    }

    void operator()(glframework& glf)
    {
        program.use();

        if (timeUniform)
        {
            *timeUniform = static_cast<float>(glfwGetTime());
        }

        if(mouseUniform)
        {
            *mouseUniform = glf.getCursorPosition();
        }

        if(resolutionUniform)
        {
            *resolutionUniform = glf.getWindowSize();
        }

        glBindVertexArray(vertexArrayID);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle

        glBindVertexArray(0);
    }

    std::string getTitle()
    {
        return title;
    }

private:
    GLuint vertexArrayID;
    glv::ShaderProgram program;
    glv::UniformDeclaration timeUniform;
    glv::UniformDeclaration mouseUniform;
    glv::UniformDeclaration resolutionUniform;
    std::string title;
};

int main(int argc, char **argv)
{
    glframework glfw;
    if(!glfw.init())
    {
        std::cerr << "Cannot initialise GLFW" << std::endl;
        return false;
    }
    glfw.makeCurrent();
    gladLoadGL();

    std::cout << "OpenGL version " << GLVersion.major << "." << GLVersion.minor << std::endl;

    GlslViewer viewer(argc, argv);
    glfw.setTitle(viewer.getTitle());

    /* Loop until the user closes the window */
    while (glfw.shouldContinue())
    {
        viewer(glfw);
        glfw.swapAndPollEvents();
    }
    return 0;
}
