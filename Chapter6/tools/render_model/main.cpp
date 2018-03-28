
#include "ObjLoader.h"
#include "camera.h"
#include "shader.h"
#include "common.h"

float g_rotateX = 0.0f;
float g_rotateY = 0.0f;
float g_zDepth = 5.0f;

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
            g_rotateX = 0.0f;
            g_rotateY = 0.0f;
            break;
        case GLFW_KEY_Z:
            g_rotateX += 0.01f;
            break;
        case GLFW_KEY_X:
            g_rotateX -= 0.01f;
            break;
        case GLFW_KEY_A:
            g_rotateY += 0.01f;
            break;
        case GLFW_KEY_S:
            g_rotateY -= 0.01f;
            break;
        case GLFW_KEY_UP:
            g_zDepth += 0.5f;
            break;
        case GLFW_KEY_DOWN:
            g_zDepth -= 0.5f;
            break;
        default:
            break;
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printf("Usage: ./render_model <model.obj> <render-type>\n");
        exit(EXIT_FAILURE);
    }

    std::string l_renderType(argv[2]);
    if ("lines" != l_renderType &&
        "points" != l_renderType &&
        "triangles" != l_renderType)
    {
        printf("Need a valid render type: 'lines', 'points', or 'triangles'\n");
        exit(EXIT_FAILURE);
    }

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

    const int WINDOWS_WIDTH = 1280;
    const int WINDOWS_HEIGHT = 720;
    l_window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Chapter 6 - Model Loading", NULL, NULL);
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

    // keyboard controls
    glfwSetInputMode(l_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(l_window, KeyCallback);

    CObjLoader l_loader;
    if (l_loader.LoadAsset(argv[1]) != 0)
    {
        fprintf(stderr, "Failed to Load the 3D file\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    GLuint l_programId = LoadShaders("../tools/render_model/pointcloud.vert", "../tools/render_model/pointcloud.frag");

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // get the location for our "Model View Projection" uniform variable
	GLuint l_mvpMatrixId = glGetUniformLocation(l_programId, "MVP");

    // use our shader
    glUseProgram(l_programId);

    // use a large buffer to store the entire scene
    GLfloat	*l_vertexBufferData = (GLfloat*) malloc(l_loader.GetNumVertices()*sizeof(GLfloat));
    l_loader.LoadVertices(l_vertexBufferData);

    // Get the location of the attribute variables
    GLint l_attributeVertexLoc = glGetAttribLocation(l_programId, "vertexPosition_modelspace");

    // Generate the vertex array object VAO (dependency GLEW)
    GLuint l_vertexArrayId;
    glGenVertexArrays(1, &l_vertexArrayId);
    glBindVertexArray(l_vertexArrayId);

    // Init the vertex buffer memory (similar to malloc)
    GLuint l_vertexBuffer;
    glGenBuffers(1, &l_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, l_vertexBuffer);
    // Load data
    glBufferData(GL_ARRAY_BUFFER, l_loader.GetNumVertices()*sizeof(GLfloat), l_vertexBufferData, GL_STATIC_DRAW);

    // 1st attribute buffer : vertices for position
    glEnableVertexAttribArray(l_attributeVertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, l_vertexBuffer);
    glVertexAttribPointer(l_attributeVertexLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPointSize(3.0f);

    bool l_stereo = true;
    CCamera l_camera;

    const float l_IPD = 0.65f;
    while (!glfwWindowShouldClose(l_window))
    {
        // clear screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        int l_width, l_height;
        /*
         * You are passing the window size, which is in screen coordinates,
         * to glViewport, which works with pixels. On OS X with a Retina display,
         * and possibly on other platforms in the future, screen coordinates and
         * pixels do not map 1:1. Use the framebuffer size, which is in pixels,
         * instead of the window size. See the Window handling guide for details.
         */
         glfwGetFramebufferSize(l_window, &l_width, &l_height);

         if (l_stereo)
         {
             // left eye, left half of screen
             bool l_isLeftEye = true;
             glViewport(0, 0, l_width/2, l_height);

             l_camera.ComputeStereoMatricesFromWindow(l_window, l_IPD, g_zDepth, l_isLeftEye);

             // how to project z from the view matrix
             glm::mat4 l_projectionMatrix = l_camera.GetProjectionMatrix();

             // where the camera is wrt the world
             glm::mat4 l_viewMatrix = l_camera.GetViewMatrix();

             // where the model is wrt the world
             glm::mat4 l_modelMatrix = glm::mat4(1.0);

             l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
             l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateX, glm::vec3(1.0f, 0.0f, 0.0f));

             // MVP = ModelViewProjection matrix = projection * view * model
             glm::mat4 l_mvp = l_projectionMatrix * l_viewMatrix * l_modelMatrix;

             // send our transformation to the currently bound shader
             // in the "MVP" uniform variable
             // void glUniformMatrix4fv(	GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
             glUniformMatrix4fv(l_mvpMatrixId, 1, GL_FALSE, &l_mvp[0][0]);

             if ("lines" == l_renderType)
             {
                 l_loader.Draw(GL_LINES);
             }
             else if ("points" == l_renderType)
             {
                 l_loader.Draw(GL_POINTS);
             }
             else if ("triangles" == l_renderType)
             {
                 l_loader.Draw(GL_TRIANGLES);
             }

             // Right eye, right half of screen
             l_isLeftEye = false;
             glViewport((l_width / 2), 0, (l_width / 2), l_height);

             l_camera.ComputeStereoMatricesFromWindow(l_window, l_IPD, g_zDepth, l_isLeftEye);

             // how to project z from the view matrix
             l_projectionMatrix = l_camera.GetProjectionMatrix();

             // where the camera is wrt the world
             l_viewMatrix = l_camera.GetViewMatrix();

             // where the model is wrt the world
             l_modelMatrix = glm::mat4(1.0);

             l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
             l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateX, glm::vec3(1.0f, 0.0f, 0.0f));

             // MVP = ModelViewProjection matrix = projection * view * model
             l_mvp = l_projectionMatrix * l_viewMatrix * l_modelMatrix;

             // send our transformation to the currently bound shader
             // in the "MVP" uniform variable
             // void glUniformMatrix4fv(	GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
             glUniformMatrix4fv(l_mvpMatrixId, 1, GL_FALSE, &l_mvp[0][0]);

             if ("lines" == l_renderType)
             {
                 l_loader.Draw(GL_LINES);
             }
             else if ("points" == l_renderType)
             {
                 l_loader.Draw(GL_POINTS);
             }
             else if ("triangles" == l_renderType)
             {
                 l_loader.Draw(GL_TRIANGLES);
             }
         }
         else
         {
             // Not stereo
             glViewport(0, 0, l_width, l_height);
             l_camera.ComputeMatricesFromWindow(l_window);

             // how to project z from the view matrix
             glm::mat4 l_projectionMatrix = l_camera.GetProjectionMatrix();

             // where the camera is wrt the world
             glm::mat4 l_viewMatrix = l_camera.GetViewMatrix();

             // where the model is wrt the world
             glm::mat4 l_modelMatrix = glm::mat4(1.0);

             l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
             l_modelMatrix = glm::rotate(l_modelMatrix, glm::pi<float>() * g_rotateX, glm::vec3(1.0f, 0.0f, 0.0f));

             // MVP = ModelViewProjection matrix = projection * view * model
             glm::mat4 l_mvp = l_projectionMatrix * l_viewMatrix * l_modelMatrix;

             // send our transformation to the currently bound shader
             // in the "MVP" uniform variable
             // void glUniformMatrix4fv(	GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
             glUniformMatrix4fv(l_mvpMatrixId, 1, GL_FALSE, &l_mvp[0][0]);

             if ("lines" == l_renderType)
             {
                 l_loader.Draw(GL_LINES);
             }
             else if ("points" == l_renderType)
             {
                 l_loader.Draw(GL_POINTS);
             }
             else if ("triangles" == l_renderType)
             {
                 l_loader.Draw(GL_TRIANGLES);
             }
         }

        // Swap the front and back buffers (GLFW uses double buffering) to update the screen and process all pending events:
        glfwSwapBuffers(l_window);
        glfwPollEvents();
    }

    // Release the memory and terminate the GLFW library.
    glfwDestroyWindow(l_window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
