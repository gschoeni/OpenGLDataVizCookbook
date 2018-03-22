
#include "camera.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

CCamera::CCamera()
{
    m_position = glm::vec3( 0, 0, 2 );
    m_speed = 3.0f; // 3 units / second
    m_initialFov = glm::pi<float>()*0.4f;
    m_lastTime = glfwGetTime();
}

glm::mat4 CCamera::GetViewMatrix()
{
  return m_viewMatrix;
}

glm::mat4 CCamera::GetProjectionMatrix()
{
  return m_projectionMatrix;
}

void CCamera::ComputeMatricesFromWindow(GLFWwindow* a_window)
{
    // compute time difference between last frame and this one
    double l_currentTime = glfwGetTime();
    float l_deltaTime = float(l_currentTime - m_lastTime);
    m_lastTime = l_currentTime;

    // Window size
    int l_width, l_height;
    glfwGetWindowSize(a_window, &l_width, &l_height);

    // Direction vector for movement
    glm::vec3 l_direction(0, 0, -1);
    // up vector
    glm::vec3 l_up(0, -1, 0);
    if (GLFW_PRESS == glfwGetKey(a_window, GLFW_KEY_UP))
    {
        m_position += l_direction * l_deltaTime * m_speed;
    }
    else if (GLFW_PRESS == glfwGetKey(a_window, GLFW_KEY_DOWN))
    {
        m_position -= l_direction * l_deltaTime * m_speed;
    }
    else if (GLFW_PRESS == glfwGetKey(a_window, GLFW_KEY_RIGHT))
    {
        m_initialFov -= 0.1 * l_deltaTime * m_speed;
    }
    else if (GLFW_PRESS == glfwGetKey(a_window, GLFW_KEY_LEFT))
    {
        m_initialFov += 0.1 * l_deltaTime * m_speed;
    }

    // update projection matrix: Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
    float l_aspectRatio = float(l_width) / float(l_height);
    m_projectionMatrix = glm::perspective(m_initialFov, l_aspectRatio, 0.1f, 100.0f);

    // update the view matrix
    m_viewMatrix = glm::lookAt(
        m_position, // camera position
        m_position+l_direction, // viewing direction
        l_up // up direction
    );
}

void CCamera::ComputeStereoMatricesFromWindow(
    GLFWwindow* a_window, float a_IOD, float a_zDepth, bool a_isLeftEye)
{
    int l_width, l_height;
    glfwGetWindowSize(a_window, &l_width, &l_height);

    glm::vec3 l_up(0, -1, 0);
    glm::vec3 l_zDir(0, 0, -1);
    // mirror the params with the right eye
    float l_leftRightDirection = -1.0f;
    if (a_isLeftEye)
    {
        l_leftRightDirection = 1.0f;
    }

    float l_aspectRatio = (float)l_width / (float) l_height;
    float l_nearZ = 1.0;
    float l_farZ = 100.0;
    double l_frustumShift = (a_IOD / 2.0) * l_nearZ / a_zDepth;
    float l_top = tan(m_initialFov / 2.0) * l_nearZ;
    float l_right = (l_aspectRatio * l_top) + (l_frustumShift * l_leftRightDirection);
    float l_left = (-l_aspectRatio * l_top) + (l_frustumShift * l_leftRightDirection);
    float l_bottom = -l_top;

    m_projectionMatrix = glm::frustum(l_left, l_right, l_bottom, l_top, l_nearZ, l_farZ);

    m_viewMatrix = glm::lookAt(
        m_position - l_zDir + glm::vec3((l_leftRightDirection*a_IOD / 2.0), 0, 0), // eye position
        m_position + glm::vec3((l_leftRightDirection*a_IOD / 2), 0, 0), //centre position
        l_up // up direction
    );
}
