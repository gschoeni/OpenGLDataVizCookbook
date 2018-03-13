#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES // M_PI constant
#include <math.h>

// Window size
const int WINDOWS_WIDTH = 1280;
const int WINDOWS_HEIGHT = 720;
GLfloat g_alpha = 210.0f, g_beta = -70.0f, g_zoom = 2.0f;
GLboolean g_locked = GL_FALSE;
bool g_freeze = false;

int g_cursorX = 0;
int g_cursorY = 0;

//basic structure for a Vertex to simplify data passing
typedef struct
{
    GLfloat x, y, z;
    GLfloat r, g, b, a;
} Vertex;

//structure for data stream
typedef struct
{
    GLfloat x, y, z;
} Data;

// Camera params depend on window size
// This is the callback that gives us updates to window size
void FrameBufferSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
    const double DEG2RAD = 3.14159265 / 180;
    // Preset the camera parameters:
    // the vertical field of view angle (fovY)
    const float l_fovY = 45.0f;
    // the distance to the Near clipping plane (front)
    const float l_front = 0.1f;
    // the distance to Far clipping plane (back)
    const float l_back = 128.0f;
    // the screen aspect ratio (width/height)
    float l_ratio = 1.0f;
    if (a_height > 0)
        l_ratio = (float) a_width / (float) a_height;

    // Setup the viewport of the virtual camera (using the window size)
    glViewport(0, 0, a_width, a_height);

    // Specify that matrix operations should be applied to the projection matrix stack
    glMatrixMode(GL_PROJECTION);

    // Reset the matrix to it's default state (identity)
    glLoadIdentity();

    double l_tangent = tan(l_fovY/2 * DEG2RAD);   // tangent of half fovY
	double l_height_f = l_front * l_tangent;          // half height of near plane
	double l_width_f = l_height_f * l_ratio;      // half width of near plane

	//Create the projection matrix based on the front
	//clipping plane and the location of the corners
	glFrustum(-l_width_f, l_width_f, -l_height_f, l_height_f, l_front, l_back);
}

void Draw2DHeatMap(const Data *data, int num_points)
{
    // locate the maximum and minimum values in the dataset
    float max_value = -999.9f;
    float min_value = 999.9f;
    for (int i=0; i<num_points; i++)
    {
        const Data d = data[i];
        if( d.z > max_value)
        {
            max_value = d.z;
        }
        if (d.z < min_value)
        {
            min_value = d.z;
        }
    }
    const float halfmax = (max_value + min_value) / 2;

    // display the result
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    float transparency = 0.25f;

    for (int i = 0; i<num_points; i++)
    {
        const Data d = data[i];
        float value = d.z;
        float b = 1.0f - value/halfmax;
        float r = value/halfmax - 1.0f;
        if(b < 0)
        {
            b=0;
        }
        if(r < 0)
        {
            r=0;
        }
        float g = 1.0f - b - r;

        glColor4f(r, g, b, transparency);
        glVertex3f(d.x, d.y, d.z);
    }
    glEnd();
}

void GaussianDemo(float sigma)
{
    //construct a 400x400 grid
    const int grid_x = 400;
    const int grid_y = 400;
    const int num_points = grid_x*grid_y;
    Data *data=(Data*)malloc(sizeof(Data)*num_points);
    int data_counter=0;
    for(int x = -grid_x/2; x<grid_x/2; x+=1){
        for(int y = -grid_y/2; y<grid_y/2; y+=1){
            float x_data = 2.0f*x/grid_x;
            float y_data = 2.0f*y/grid_y;
            //compute the height z based on a 2-D Gaussian function.
            float z_data = exp(-0.5f*(x_data*x_data)/(sigma*sigma) - 0.5f*(y_data*y_data)/(sigma*sigma))/(sigma*sigma*2.0f*M_PI);
            data[data_counter].x = x_data;
            data[data_counter].y = y_data;
            data[data_counter].z = z_data;
            data_counter++;
        }
    }
    //visualize the result using a 2D heat map
    Draw2DHeatMap(data, num_points);
    free(data);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_SPACE:
            g_freeze=!g_freeze;
            break;
        case GLFW_KEY_LEFT:
            g_alpha += 5.0f;
            break;
        case GLFW_KEY_RIGHT:
            g_alpha -= 5.0f;
            break;
        case GLFW_KEY_UP:
            g_beta -= 5.0f;
            break;
        case GLFW_KEY_DOWN:
            g_beta += 5.0f;
            break;
        case GLFW_KEY_PAGE_UP:
            g_zoom -= 0.25f;
            if (g_zoom < 0.0f)
                g_zoom = 0.0f;
            break;
        case GLFW_KEY_PAGE_DOWN:
            g_zoom += 0.25f;
            break;
        default:
            break;
    }
}

void MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        g_locked = GL_TRUE;
    }
    else
    {
        g_locked = GL_FALSE;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
    //if the mouse button is pressed
    if (g_locked)
    {
        g_alpha += (GLfloat) (x - g_cursorX) / 10.0f;
        g_beta += (GLfloat) (y - g_cursorY) / 10.0f;
    }
    //update the cursor position
    g_cursorX = (int) x;
    g_cursorY = (int) y;
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
    g_zoom += (float) y / 4.0f;
    if (g_zoom < 0.0f)
        g_zoom = 0.0f;
}

void DrawOrigin(){
    float transparency = 0.5f;
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    //draw a red line for the x-axis
    glColor4f(1.0f, 0.0f, 0.0f, transparency);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, transparency);
    glVertex3f(0.3f, 0.0f, 0.0f);

    //draw a green line for the y-axis
    glColor4f(0.0f, 1.0f, 0.0f, transparency);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor4f(0.0f, 1.0f, 0.0f, transparency);
    glVertex3f(0.0f, 0.0f, 0.3f);

    //draw a blue line for the z-axis
    glColor4f(0.0f, 0.0f, 1.0f, transparency);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor4f(0.0f, 0.0f, 1.0f, transparency);
    glVertex3f(0.0f, 0.3f, 0.0f);
    glEnd();
}

int main(int argc, char const *argv[])
{
    GLFWwindow* l_window;
    int l_width, l_height;

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    l_window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Chapter 3", NULL, NULL);
    if (!l_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //initialize the callbacks for event handling
    //keyboard input callback
    glfwSetKeyCallback(l_window, KeyCallback);
    //framebuffer size callback - i.e., window resizing
    glfwSetFramebufferSizeCallback(l_window, FrameBufferSizeCallback);
    //mouse button callback
    glfwSetMouseButtonCallback(l_window, MouseCallback);
    //mouse movement callback
    glfwSetCursorPosCallback(l_window, CursorPositionCallback);
    //mouse scroll callback
    glfwSetScrollCallback(l_window, ScrollCallback);

    // Make sure window is on the current calling thread
    glfwMakeContextCurrent(l_window);
    glfwSwapInterval(1);

    //get the frame buffer (window) size
    glfwGetFramebufferSize(l_window, &l_width, &l_height);
    // initial call to the framebuffer callback, and initialize the OpenGL
    // camera and other properties there
    FrameBufferSizeCallback(l_window, l_width, l_height);

    // enable anti-aliasing
    glEnable(GL_BLEND);
    // smooth the points
    glEnable(GL_LINE_SMOOTH);
    // smooth the lines
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    // needed for alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);

    //initialize all parameters
    float sigma = 0.1f;
    float sign = 1.0f;
    float step_size = 0.01f;

    while (!glfwWindowShouldClose(l_window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //draw the scene
        //switch to modelview so the tranformation applies to entire model
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //move the object back and forth based on the zoom level
        glTranslatef(0.0, 0.0, -g_zoom);
        // rotate beta degrees around the x-axis
        glRotatef(g_beta, 1.0, 0.0, 0.0);
        // rotate alpha degrees around the z-axis
        glRotatef(g_alpha, 0.0, 0.0, 1.0);

        //draw the origin with the x,y,z axes for visualization
        DrawOrigin();

        //draw the gaussian distribution in 2D with heatmap
        if(!g_freeze){
            sigma=sigma+sign*step_size;
            if(sigma>1.0f){
                sign = -1.0f;
            }
            if(sigma<0.1){
                sign = 1.0f;
            }
        }
        GaussianDemo(sigma);

        // Swap the front and back buffers (GLFW uses double buffering) to update the screen and process all pending events:
        glfwSwapBuffers(l_window);
        glfwPollEvents();
    }

    // Release the memory and terminate the GLFW library.
    glfwDestroyWindow(l_window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
