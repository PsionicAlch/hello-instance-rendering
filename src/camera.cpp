#include "camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
                                                                            m_Position(position), 
                                                                            m_WorldUp(up),
                                                                            m_Yaw(yaw),
                                                                            m_Pitch(pitch),
                                                                            m_MovementSpeed(SPEED), 
                                                                            m_MouseSensitivity(SENSITIVITY), 
                                                                            m_Zoom(ZOOM) {
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
                                                                                                                m_Position(glm::vec3(posX, posY, posZ)),
                                                                                                                m_WorldUp(glm::vec3(upX, upY, upZ)),
                                                                                                                m_Yaw(yaw),
                                                                                                                m_Pitch(pitch),                            
                                                                                                                m_MovementSpeed(SPEED),
                                                                                                                m_MouseSensitivity(SENSITIVITY),
                                                                                                                m_Zoom(ZOOM) {
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = m_MovementSpeed * deltaTime;

    switch (direction) {
    case FORWARD:
        m_Position += m_Front * velocity;
        break;
    case BACKWARD:
        m_Position -= m_Front * velocity;
        break;
    case LEFT:
        m_Position -= m_Right * velocity;
        break;
    case RIGHT:
        m_Position += m_Right * velocity;
        break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw   += xoffset;
    m_Pitch += yoffset;

    if (constrainPitch) {
        if (m_Pitch > 89.0f) {
            m_Pitch = 89.0f;
        } else if (m_Pitch < -89.0f) {
            m_Pitch = -89.0f;
        }
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    m_Zoom -= yoffset;

    if (m_Zoom < 1.0f) {
        m_Zoom = 1.0f;
    } else if (m_Zoom > 45.0f) {
        m_Zoom = 45.0f;
    }
}

float Camera::GetZoom() {
    return m_Zoom;
}

glm::vec3 Camera::GetPosition() {
    return m_Position;
}

glm::vec3 Camera::GetFront() {
    return m_Front;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)));
    front.y = static_cast<float>(sin(glm::radians(m_Pitch)));
    front.z = static_cast<float>(sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)));

    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}