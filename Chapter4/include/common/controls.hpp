#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "common/common.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CControls
{
public:
    CControls();

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    void ComputeMatricesFromWindow(GLFWwindow* a_window);

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


#endif
