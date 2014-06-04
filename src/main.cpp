#include <cassert>
#include <gl.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

#include "ObjModel.hpp"

static void error_callback(int error, const char* description)
{
    std::cerr << "[" << error << "] " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::clog << "Usage: " << argv[0] << " [filename]" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs.is_open())
    {
        std::clog << "Cannot open file " << argv[1] << std::endl;
        return 1;
    }

    vfm::ObjModel model;
    ifs >> model;
    ifs.close();

    std::cout << std::setw(10) << "Vertices: " << model.vertices.size() << std::endl;
    std::cout << std::setw(10) << "Normals: " << model.normals.size() << std::endl;
    std::cout << std::setw(10) << "Faces: " << model.faces.size() << std::endl;


    glfwSetErrorCallback(error_callback);
    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    if(!gl3wInit())
    {
      //Problem: glewInit failed, something is seriously wrong.
      std::cerr<<"glewInit failed, aborting."<< std::endl;
      return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    // BEGIN INITIALIZATION

    // Create and bind a VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and bind a BO for vertex data
    GLuint vbuffer;
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

    // copy data into the buffer object
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(vfm::Vertex4d), &model.vertices[0], GL_STATIC_DRAW);

    // set up vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vfm::Vertex4d), 0);

    // Create and bind a BO for index data
    GLuint ibuffer;
    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);

    // copy data into the buffer object
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.faces.size() * sizeof(vfm::TriangleFace), &model.faces[0], GL_STATIC_DRAW);

    // At this point the VAO is set up with two vertex attributes
    // referencing the same buffer object, and another buffer object
    // as source for index data. We can now unbind the VAO, go do
    // something else, and bind it again later when we want to render
    // with it.

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    // END INITIALIZATION

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // This is it. Binding the VAO again restores all buffer
        // bindings and attribute settings that were previously set up
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, model.faces.size() * 3 , GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
