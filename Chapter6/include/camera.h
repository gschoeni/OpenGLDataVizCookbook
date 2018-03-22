
#pragma once

#include "common.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CCamera
{
public:
    CCamera();

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    void ComputeMatricesFromWindow(GLFWwindow* a_window);
    void ComputeStereoMatricesFromWindow(GLFWwindow* a_window, float a_IOD, float a_zDepth, bool a_isLeftEye);

private:
    double m_lastTime;
    // initial position of the camera
    glm::vec3 m_position;
    float m_speed;
    float m_initialFov;
    // the view matrix and projection matrix
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};
