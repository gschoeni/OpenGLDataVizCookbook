
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

// Globals
GLFWwindow* g_window;
const int WINDOWS_WIDTH = 1280;
const int WINDOWS_HEIGHT = 720;
float g_aspectRatio = 16 / 9.0f;
float g_zOffset = -1.0f;
float g_rotateY = 0.0f;
float g_rotateX = 0.0f;

// Our vertices
static const GLfloat g_vertexBufferObj1[] = {
    -1.0, -1.0f, g_zOffset,
     0.0, -1.0f, g_zOffset,
     -1.0,  0.0f, g_zOffset,
    0.0, -1.0f, g_zOffset,
     0.0,  0.0f, g_zOffset,
    -1.0,  0.0f, g_zOffset
};

static const GLfloat g_vertexBufferObj2[] = {
    0.0, 0.0f, g_zOffset,
    1.0, 0.0f, g_zOffset,
    0.0, 1.0f, g_zOffset,
    1.0, 0.0f, g_zOffset,
    1.0, 1.0f, g_zOffset,
    0.0, 1.0f, g_zOffset
};

// UV map for the vertices
static const GLfloat g_uvBufferObj1[] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

static const GLfloat g_uvBufferObj2[] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

static void KeyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
    if (a_action != GLFW_PRESS && a_action != GLFW_REPEAT)
    {
        return;
    }

    switch (a_key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(a_window, GL_TRUE);
            break;
        case GLFW_KEY_SPACE:
            g_rotateX = 0;
            g_rotateY = 0;
            break;
        case GLFW_KEY_Z:
            g_rotateX += 0.01;
            break;
        case GLFW_KEY_X:
            g_rotateX -= 0.01;
            break;
        case GLFW_KEY_A:
            g_rotateY += 0.01;
            break;
        case GLFW_KEY_S:
            g_rotateY -= 0.01;
            break;
        default:
            break;
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: ./texture_mapping <texture1.png> <texture2.png>\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* l_window;
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    // enable anti-aliasing 4x with GLFW
    glfwWindowHint(GLFW_SAMPLES, 4);
    // To enable OpenGL 3.2 support
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    l_window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Chapter 4 - GLSL", NULL, NULL);
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

    //keyboard input callback
    glfwSetInputMode(l_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(l_window, KeyCallback);

    // Set a black background and enable alpha blending for various visual effects:
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Setup shader programs
    GLuint l_programId = LoadShaders("../tools/texture_mapping/texture.vert", "../tools/texture_mapping/texture.frag");

    std::string l_imageFilePath1(argv[1]);
    std::string l_imageFilePath2(argv[2]);

    int l_image1Width, l_image1Height, l_channels;
    printf("Trying to load texture: %s\n", l_imageFilePath1.c_str());
    GLuint l_texture1Id = LoadImageToTexture(l_imageFilePath1.c_str(), &l_image1Width, &l_image1Height, &l_channels);
    if (!l_texture1Id)
    {
        fprintf(stderr, "Could not load texture: %s\n", l_imageFilePath1.c_str());
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    printf("loaded texture1 with id: %u\n", l_texture1Id);

    int l_image2Width, l_image2Height;
    GLuint l_texture2Id = LoadImageToTexture(l_imageFilePath2.c_str(), &l_image2Width, &l_image2Height, &l_channels);
    if (!l_texture2Id)
    {
        fprintf(stderr, "Could not load texture: %s\n", l_imageFilePath2.c_str());
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    printf("loaded texture2 with id: %u\n", l_texture2Id);

    // set aspect ratio to that of the image
    g_aspectRatio = (float)(l_image1Width + l_image2Width)/(float)(l_image1Height + l_image2Height);
    printf("Image 1 size %dx%d Image 2 size %dx%d aspect %f\n", l_image1Width, l_image1Height, l_image2Width, l_image2Height, g_aspectRatio);

    // Get the locations of the specific variables in the shader programs

    // ** Uniform **
    // https://www.khronos.org/opengl/wiki/Type_Qualifier_(GLSL)#Storage_qualifier
    // get the location for our "MVP" uniform variable
    // "uniforms" are global variables that can be passed into a shader
    // their value does not change between multiple executions of a
    // shader during the rendering of a primitive (ie: during a glDraw* call)
    // They are constant, but not compile-time constant (so not const).
    GLuint l_matrixId = glGetUniformLocation(l_programId, "MVP"); // (this is in the vertex shader)

    // Get a handler for our "textureSampler" uniform
    GLuint l_textureSampleId = glGetUniformLocation(l_programId, "textureSampler"); // (this is in the fragment shader)

    // Get the attribute ids for the variables for the vertex attributes (inputs to the vertex shader)
    GLint l_attribVertex, l_attribUV;
    l_attribVertex = glGetAttribLocation(l_programId, "vertexPosition_modelspace");
    l_attribUV = glGetAttribLocation(l_programId, "vertexUV");

    // Use the shader program
    glUseProgram(l_programId);

    // Our first texture
    // Define our Vertex Array Objects (VAO)
    GLuint l_vertexArrayObject1;
    // Define our Vertex Buffer Objects
    GLuint l_vertexBufferObject1;
    GLuint l_uvBufferObject1;
    {
        // glGenVertexArrays(GLsizei n, GLuint *arrays) creates the vertex array
        // n specifies number of arrays you want to create, in this case just 1
        glGenVertexArrays(1, &l_vertexArrayObject1);

        // glBindVertexArray makes this new array active if it is not already
        // You can think of it as:
        // if (opengl->buffers[l_vertexId] == null)
        //     opengl->buffers[l_vertexId] = new Buffer()
        glBindVertexArray(l_vertexArrayObject1);

        /*
        Pattern for buffer allocation

        glGenBuffers - generates an id/name for the buffer
            e.g. buffer = 2
        glBindBuffer - create buffer if !created
            e.g. if (opengl->buffers[l_vertexId] == null)
                    opengl->buffers[l_vertexId] = new Buffer()
        glBufferData - copy data into active buffer
            e.g. opengl->current_array_buffer->data = new byte[sizeof(points)]
                 memcpy(opengl->current_array_buffer->data, points, sizeof(points))
        */

        glGenBuffers(1, &l_vertexBufferObject1);
        glBindBuffer(GL_ARRAY_BUFFER, l_vertexBufferObject1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexBufferObj1), g_vertexBufferObj1, GL_STATIC_DRAW);

        glGenBuffers(1, &l_uvBufferObject1);
        glBindBuffer(GL_ARRAY_BUFFER, l_uvBufferObject1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uvBufferObj1), g_uvBufferObj1, GL_STATIC_DRAW);

        // 1st attribute buffer: vertices for position
        glEnableVertexAttribArray(l_attribVertex);
        glBindBuffer(GL_ARRAY_BUFFER, l_vertexBufferObject1);
        glVertexAttribPointer(l_attribVertex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer: UVs mapping
        glEnableVertexAttribArray(l_attribUV);
        glBindBuffer(GL_ARRAY_BUFFER, l_uvBufferObject1);
        glVertexAttribPointer(l_attribUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Bind all texture units and attribute buffers
        // binds our texture in Texture Unit 0
        printf("Trying to activate texture 1\n");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, l_texture1Id);
        glUniform1i(l_textureSampleId, 0);
        printf("Done activating texture 1\n");
    }

    // Our second texture
    GLuint l_vertexArrayObject2;
    GLuint l_vertexBufferObject2;
    GLuint l_uvBufferObject2;
    {
        glGenVertexArrays(1, &l_vertexArrayObject2);
        glBindVertexArray(l_vertexArrayObject2);

        glGenBuffers(1, &l_vertexBufferObject2);
        glBindBuffer(GL_ARRAY_BUFFER, l_vertexBufferObject2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexBufferObj2), g_vertexBufferObj2, GL_STATIC_DRAW);

        glGenBuffers(1, &l_uvBufferObject2);
        glBindBuffer(GL_ARRAY_BUFFER, l_uvBufferObject2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uvBufferObj2), g_uvBufferObj2, GL_STATIC_DRAW);

        // 1st attribute buffer: vertices for position
        glEnableVertexAttribArray(l_attribVertex);
        glBindBuffer(GL_ARRAY_BUFFER, l_vertexBufferObject2);
        glVertexAttribPointer(l_attribVertex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer: UVs mapping
        glEnableVertexAttribArray(l_attribUV);
        glBindBuffer(GL_ARRAY_BUFFER, l_uvBufferObject2);
        glVertexAttribPointer(l_attribUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Bind all texture units and attribute buffers
        // binds our texture in Texture Unit 0
        printf("Trying to activate texture 2\n");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, l_texture2Id);
        glUniform1i(l_textureSampleId, 0);
        printf("Done activating texture 2\n");
    }

    // Create controls object to manage the view
    CControls l_controls;

    // While the window is open
    while (!glfwWindowShouldClose(l_window) &&
           GLFW_PRESS != glfwGetKey(l_window, GLFW_KEY_ESCAPE))
    {
        // Clear the screen && depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Compute the transforms and store the information in the shader variables
        l_controls.ComputeMatricesFromWindow(l_window);

        // how to project z from the view matrix
        glm::mat4 l_projectionMatrix = l_controls.GetProjectionMatrix();

        // where the camera is wrt the world
        glm::mat4 l_viewMatrix = l_controls.GetViewMatrix();

        // where the model is wrt the world
        glm::mat4 l_modelMatrix = glm::mat4(1.0);
        l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
        l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateX, glm::vec3(1.0f, 0.0f, 0.0f));

        // MVP = ModelViewProjection matrix = projection * view * model
        glm::mat4 l_mvp = l_projectionMatrix * l_viewMatrix * l_modelMatrix;

        // send our transformation to the currently bound shader
        // in the "MVP" uniform variable
        // void glUniformMatrix4fv(	GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        glUniformMatrix4fv(l_matrixId, 1, GL_FALSE, &l_mvp[0][0]);

        // Draw square
        glBindVertexArray(l_vertexArrayObject1);
        glBindTexture(GL_TEXTURE_2D, l_texture1Id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(l_vertexArrayObject2);
        glBindTexture(GL_TEXTURE_2D, l_texture2Id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Swap the front and back buffers (GLFW uses double buffering) to update the screen and process all pending events:
        glfwSwapBuffers(l_window);
        glfwPollEvents();
    }

    // Release the memory and terminate the GLFW library.
    glDisableVertexAttribArray(l_attribVertex);
    glDisableVertexAttribArray(l_attribUV);
    // Clean up VBO and shader program
    // glDeleteBuffers(1, &l_vertexBuffer);
    // glDeleteBuffers(1, &l_uvBuffer);
    glDeleteProgram(l_programId);
    glDeleteTextures(1, &l_texture1Id);
    glDeleteVertexArrays(1, &l_vertexArrayObject1);

    glfwDestroyWindow(l_window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
