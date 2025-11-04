#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <algorithm> 
#include <glm/common.hpp>

#include "shader.h"
#include "cube.h"
#include "robot.h"
using namespace std;


// Callback function for window resize
void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}

// Process keyboard input
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Graphics Project", NULL, NULL);
    if (window == NULL) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Print OpenGL version
    cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;  
    cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;

    glEnable(GL_DEPTH_TEST);

    //shader
    GLuint shaderProgram = loadShaderProgram("shaders/vertex_shader.glsl","shaders/fragment_shader.glsl");

    GLuint cubeVAO = createCube();

    glm::vec3 camPos = glm::vec3(3.0f, 3.0f, 5.0f);
    glm::vec3 camTar = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(camPos, camTar, camUp);

    // projection
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    // lighting
    glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
    glm::vec3 lightCol(1.0f, 1.0f, 1.0f); 
    glm::vec3 objectCol(0.8f, 0.3f, 0.3f);

    // shader & uniforms
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightCol"), 1, glm::value_ptr(lightCol));
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectCol"), 1, glm::value_ptr(objectCol));
    glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, glm::value_ptr(camPos));



    // Set viewport
    glViewport(0, 0, 800, 600);

    static float lastTime = glfwGetTime();

while (!glfwWindowShouldClose(window)) {
    float now = glfwGetTime();
    float deltaTime = now - lastTime;
    lastTime = now;

    processInput(window);
    updateJointsFromInput(window, deltaTime);  // keep if you use it

    // --- persistent robot pose & animation state ---
    static float hipL_Ang  = 0.0f;
    static float kneeL_Ang = 0.0f;

    static bool  idleWalk  = false;   // SPACE toggles this
    static bool  stepping  = false;   // P triggers one-step FSM
    static int   phase     = 0;       // 0 fwd, 1 hold fwd, 2 back, 3 hold neutral
    static float phaseTime = 0.0f;

    // --- key handling (debounced here so you don't need to touch processInput) ---
    // SPACE: toggle idle walk
    { static bool prev = false;
      bool now = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
      if (now && !prev) idleWalk = !idleWalk;
      prev = now;
    }
    // P: trigger one step (only if not already stepping)
    { static bool prev = false;
      bool now = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
      if (now && !prev && !stepping) { stepping = true; phase = 0; phaseTime = 0.0f; }
      prev = now;
    }
    // R: reset pose
    { static bool prev = false;
      bool now = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
      if (now && !prev) {
          hipL_Ang = 0.0f; kneeL_Ang = 0.0f;
          stepping = false; phase = 0; phaseTime = 0.0f;
          idleWalk = false;
      }
      prev = now;
    }

    // --- idle walk animation (only when toggled on) ---
    if (idleWalk) {
        float t = (float)glfwGetTime();
        float s = sinf(2.0f * t);             // -1..+1
        hipL_Ang  =  30.0f * s;               // swing
        kneeL_Ang =  40.0f * fmaxf(0.0f, s);  // bend on forward swing
    }

    // --- one-step FSM (runs until it finishes, independent of idleWalk) ---
    if (stepping) {
        const float HIP_MAX   = 35.0f;
        const float KNEE_MAX  = 45.0f;
        const float MOVE_DUR  = 0.30f;
        const float HOLD_DUR  = 0.20f;

        phaseTime += deltaTime;

        if (phase == 0) { // move forward
            float u = glm::clamp(phaseTime / MOVE_DUR, 0.0f, 1.0f);  // NOTE: glm::clamp
            float e = 0.5f - 0.5f * cosf(3.14159f * u);
            hipL_Ang  =  HIP_MAX * e;
            kneeL_Ang =  KNEE_MAX * e;
            if (u >= 1.0f) { phase = 1; phaseTime = 0.0f; }

        } else if (phase == 1) { // hold forward
            if (phaseTime >= HOLD_DUR) { phase = 2; phaseTime = 0.0f; }

        } else if (phase == 2) { // move back to neutral
            float u = glm::clamp(phaseTime / MOVE_DUR, 0.0f, 1.0f);
            float e = 0.5f - 0.5f * cosf(3.14159f * u);
            hipL_Ang  =  HIP_MAX * (1.0f - e);
            kneeL_Ang =  KNEE_MAX * (1.0f - e);
            if (u >= 1.0f) { phase = 3; phaseTime = 0.0f; }

        } else if (phase == 3) { // hold neutral
            if (phaseTime >= HOLD_DUR) { stepping = false; }
        }
    }

    // --- apply the pose once per frame ---
   setLeftLeg(hipL_Ang, kneeL_Ang);

    // --- Orbit camera update (unchanged) ---
    float t = (float)glfwGetTime();
    float radius = 6.0f;
    glm::vec3 camPos = glm::vec3(
        radius * sinf(0.4f * t),
        3.0f,
        radius * cosf(0.4f * t)
    );
    glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, glm::value_ptr(camPos));

    // --- draw ---
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    drawRobot(shaderProgram, cubeVAO, view, projection);

    glfwSwapBuffers(window);
    glfwPollEvents();

    }

    // Cleanup
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

