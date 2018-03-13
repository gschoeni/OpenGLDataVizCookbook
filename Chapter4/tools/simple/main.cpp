
#include "common/shader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char const *argv[])
{
    GLFWwindow* l_window;
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    // enable anti-aliasing 4x with GLFW
    glfwWindowHint(GLFW_SAMPLES, 4);
    // To enable OpenGL 3.2 support
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    l_window = glfwCreateWindow(640, 480, "Chapter 4 - GLSL", NULL, NULL);
    if (!l_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make sure window is on the current calling thread
    glfwMakeContextCurrent(l_window);
    glfwSwapInterval(1);

    // Initialize the GLEW library and include support for experimental drivers:
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Final to Initialize GLEW\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Setup shader programs
    GLuint l_programId = LoadShaders("../tools/simple/simple.vert", "../tools/simple/simple.frag");

    glBindFragDataLocation(l_programId, 0, "color_out");
    glUseProgram(l_programId);

    // Create Vertex Array Object
    GLuint l_vertexArray;
    glGenVertexArrays(1, &l_vertexArray);
    glBindVertexArray(l_vertexArray);

    // Set up Vertex Buffer Object (and color buffer) and copy data over
    GLuint l_vertexBuffer;
    GLuint l_colorBuffer;
    glGenBuffers(1, &l_vertexBuffer);
    glGenBuffers(1, &l_colorBuffer);

    const GLfloat l_vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
     };
     const GLfloat l_colors[]={
         0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, l_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(l_vertices), l_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, l_colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(l_colors), l_colors, GL_STATIC_DRAW);

    // Specify layout of vertex data
    GLint l_posAttr = glGetAttribLocation(l_programId, "position");
    glEnableVertexAttribArray(l_posAttr);
    glBindBuffer(GL_ARRAY_BUFFER, l_vertexBuffer);
    glVertexAttribPointer(l_posAttr, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLint l_colorAttr = glGetAttribLocation(l_programId, "color_in");
    glEnableVertexAttribArray(l_colorAttr);
    glBindBuffer(GL_ARRAY_BUFFER, l_colorBuffer);
    glVertexAttribPointer(l_colorAttr, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    while (!glfwWindowShouldClose(l_window))
    {
        // clear screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a rectangle from the 2 triangles using 6 vertices
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap the front and back buffers (GLFW uses double buffering) to update the screen and process all pending events:
        glfwSwapBuffers(l_window);
        glfwPollEvents();
    }

    //clean up the memories
    glDisableVertexAttribArray(l_posAttr);
    glDisableVertexAttribArray(l_colorAttr);
    glDeleteBuffers(1, &l_vertexBuffer);
    glDeleteBuffers(1, &l_colorBuffer);
    glDeleteVertexArrays(1, &l_vertexArray);
    glDeleteProgram(l_programId);

    // Release the memory and terminate the GLFW library.
    glfwDestroyWindow(l_window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
