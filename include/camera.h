#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// Camera modes
enum class CameraMode {
    ORBIT = 0,
    STATIC_FRONT = 1,
    FREE = 2,
    MODE_COUNT = 3
};

// Camera controller class
class Camera {
public:
    Camera();

    // Update camera (call every frame)
    void update(GLFWwindow* window, float deltaTime);

    // Get view matrix for current camera mode
    glm::mat4 getViewMatrix() const;

    // Get camera position (for lighting calculations)
    glm::vec3 getPosition() const;

    // Set camera mode
    void setMode(CameraMode mode);

    // Get current mode
    CameraMode getMode() const;

    // Cycle to next camera mode
    void nextMode();

private:
    CameraMode currentMode;

    // Orbit camera parameters
    float orbitRadius;
    float orbitSpeed;
    float orbitHeight;
    float orbitAngle;

    // Free camera parameters
    glm::vec3 freePosition;
    glm::vec3 freeTarget;
    float freeYaw;
    float freePitch;
    float freeMoveSpeed;
    float freeLookSpeed;

    // Static camera parameters
    glm::vec3 staticPosition;
    glm::vec3 staticTarget;

    // Helper functions
    glm::mat4 getOrbitViewMatrix(float currentTime) const;
    glm::mat4 getStaticViewMatrix() const;
    glm::mat4 getFreeViewMatrix() const;

    void updateFreeCamera(GLFWwindow* window, float deltaTime);
};

#endif
