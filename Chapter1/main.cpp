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

    l_window = glfwCreateWindow(640, 480, "Chapter 1", NULL, NULL);
    if (!l_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make sure window is on the current calling thread
    glfwMakeContextCurrent(l_window);
    glfwSwapInterval(1);
    
    while (!glfwWindowShouldClose(l_window))
    {
        // Set up the viewport (using the width and height of the window) and clear the screen color buffer:
        float l_ratio;
        int l_width, l_height;

        glfwGetFramebufferSize(l_window, &l_width, &l_height);
        l_ratio = (float)l_width / (float)l_height;

        glViewport(0, 0, l_width, l_height);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set up the camera matrix.
        // Note that further details on the camera model will be discussed in Chapter 3, Interactive 3D Data Visualization:
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-l_ratio, l_ratio, -1.0f, 1.0f, 1.0f, -1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Draw a rotating triangle and set a different color (red, green, and blue channels) for each vertex (x, y, and z) of the triangle. The first line rotates the triangle over time:
        glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();

        // Swap the front and back buffers (GLFW uses double buffering) to update the screen and process all pending events:
        glfwSwapBuffers(l_window);
        glfwPollEvents();
    }

    // Release the memory and terminate the GLFW library.
    glfwDestroyWindow(l_window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
