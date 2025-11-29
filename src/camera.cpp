#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

Camera::Camera()
    : currentMode(CameraMode::ORBIT),
      orbitRadius(6.0f),
      orbitSpeed(0.4f),
      orbitHeight(3.0f),
      orbitAngle(0.0f),
      freePosition(5.0f, 3.0f, 5.0f),
      freeTarget(0.0f, 1.0f, 0.0f),
      freeYaw(-135.0f),
      freePitch(-10.0f),
      freeMoveSpeed(3.0f),
      freeLookSpeed(0.1f),
      staticPosition(0.0f, 2.5f, 8.0f),
      staticTarget(0.0f, 1.5f, 0.0f)
{
}

void Camera::update(GLFWwindow* window, float deltaTime) {
    if (currentMode == CameraMode::FREE) {
        updateFreeCamera(window, deltaTime);
    }
    // Orbit camera updates based on time in getViewMatrix
    // Static camera doesn't need updates
}

glm::mat4 Camera::getViewMatrix() const {
    switch (currentMode) {
        case CameraMode::ORBIT:
            return getOrbitViewMatrix(glfwGetTime());
        case CameraMode::STATIC_FRONT:
            return getStaticViewMatrix();
        case CameraMode::FREE:
            return getFreeViewMatrix();
        default:
            return getOrbitViewMatrix(glfwGetTime());
    }
}

glm::vec3 Camera::getPosition() const {
    switch (currentMode) {
        case CameraMode::ORBIT: {
            float t = static_cast<float>(glfwGetTime());
            return glm::vec3(
                orbitRadius * sinf(orbitSpeed * t),
                orbitHeight,
                orbitRadius * cosf(orbitSpeed * t)
            );
        }
        case CameraMode::STATIC_FRONT:
            return staticPosition;
        case CameraMode::FREE:
            return freePosition;
        default:
            return glm::vec3(0.0f, 3.0f, 5.0f);
    }
}

void Camera::setMode(CameraMode mode) {
    if (mode >= CameraMode::ORBIT && mode < CameraMode::MODE_COUNT) {
        currentMode = mode;

        // Print mode change to console
        switch (currentMode) {
            case CameraMode::ORBIT:
                std::cout << "Camera: Orbit Mode" << std::endl;
                break;
            case CameraMode::STATIC_FRONT:
                std::cout << "Camera: Static Front Mode" << std::endl;
                break;
            case CameraMode::FREE:
                std::cout << "Camera: Free Mode (WASD + Arrow Keys)" << std::endl;
                break;
            default:
                break;
        }
    }
}

CameraMode Camera::getMode() const {
    return currentMode;
}

void Camera::nextMode() {
    int nextModeInt = (static_cast<int>(currentMode) + 1) % static_cast<int>(CameraMode::MODE_COUNT);
    setMode(static_cast<CameraMode>(nextModeInt));
}

glm::mat4 Camera::getOrbitViewMatrix(float currentTime) const {
    glm::vec3 camPos(
        orbitRadius * sinf(orbitSpeed * currentTime),
        orbitHeight,
        orbitRadius * cosf(orbitSpeed * currentTime)
    );
    glm::vec3 target(0.0f, 1.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    return glm::lookAt(camPos, target, up);
}

glm::mat4 Camera::getStaticViewMatrix() const {
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    return glm::lookAt(staticPosition, staticTarget, up);
}

glm::mat4 Camera::getFreeViewMatrix() const {
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    // Calculate direction from yaw and pitch
    glm::vec3 direction;
    direction.x = cos(glm::radians(freeYaw)) * cos(glm::radians(freePitch));
    direction.y = sin(glm::radians(freePitch));
    direction.z = sin(glm::radians(freeYaw)) * cos(glm::radians(freePitch));

    glm::vec3 lookTarget = freePosition + glm::normalize(direction);
    return glm::lookAt(freePosition, lookTarget, up);
}

void Camera::updateFreeCamera(GLFWwindow* window, float deltaTime) {
    // Calculate camera direction
    glm::vec3 direction;
    direction.x = cos(glm::radians(freeYaw)) * cos(glm::radians(freePitch));
    direction.y = sin(glm::radians(freePitch));
    direction.z = sin(glm::radians(freeYaw)) * cos(glm::radians(freePitch));
    glm::vec3 front = glm::normalize(direction);

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    float velocity = freeMoveSpeed * deltaTime;

    // WASD movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        freePosition += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        freePosition -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        freePosition -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        freePosition += right * velocity;

    // Q/E for up/down
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        freePosition.y += velocity;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        freePosition.y -= velocity;

    // Arrow keys for look
    float lookSpeed = 100.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        freeYaw -= lookSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        freeYaw += lookSpeed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        freePitch += lookSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        freePitch -= lookSpeed;

    // Clamp pitch to prevent gimbal lock
    if (freePitch > 89.0f)
        freePitch = 89.0f;
    if (freePitch < -89.0f)
        freePitch = -89.0f;
}
