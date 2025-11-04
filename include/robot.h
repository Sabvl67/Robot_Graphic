#pragma once
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

// Update joint angles from keyboard each frame (pass delta time in seconds)
void updateJointsFromInput(GLFWwindow* window, float dt);

// Draw the robot using your existing cube VAO.
// - program: your linked GL shader program (expects uniforms: model, view, projection, objectCol)
// - cubeVAO: VAO for your unit cube (36 vertices, drawn with glDrawArrays)
// - view, proj: your camera matrices
void drawRobot(GLuint program,
               GLuint cubeVAO,
               const glm::mat4& view,
               const glm::mat4& proj);


void setLeftLeg (float hipDeg, float kneeDeg);

//void setRightLeg (float hipDeg, float kneeDeg);

            