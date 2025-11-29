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
#include "scene.h"
#include "camera.h"
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

    // Scene manager
    SceneManager sceneManager;

    // Camera system
    Camera camera;

    // projection
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    // shader & uniforms (projection doesn't change)
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



    // Set viewport
    glViewport(0, 0, 800, 600);

    static float lastTime = glfwGetTime();

while (!glfwWindowShouldClose(window)) {
    float now = glfwGetTime();
    float deltaTime = now - lastTime;
    lastTime = now;

    processInput(window);

    // Update robot joints only if not in free camera mode (to avoid key conflicts)
    if (camera.getMode() != CameraMode::FREE) {
        updateJointsFromInput(window, deltaTime);
    }

    // --- persistent robot pose & animation state ---
    static float hipL_Ang  = 0.0f;
    static float kneeL_Ang = 0.0f;

    static bool  idleWalk  = false;   // SPACE toggles this
    static bool  stepping  = false;   // P triggers one-step FSM
    static int   phase     = 0;       // 0 fwd, 1 hold fwd, 2 back, 3 hold neutral
    static float phaseTime = 0.0f;

    // New animations
    static bool armWave = false;      // W toggles arm wave
    static bool headBob = false;      // B toggles head bobbing
    static bool torsoSway = false;    // T toggles torso sway

    // --- key handling ---
    // Scene switching: 1, 2, 3
    { static bool prev1 = false, prev2 = false, prev3 = false;
      bool now1 = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
      bool now2 = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
      bool now3 = glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS;
      if (now1 && !prev1) { sceneManager.setScene(SceneManager::DAY); cout << "Scene: Day" << endl; }
      if (now2 && !prev2) { sceneManager.setScene(SceneManager::NIGHT); cout << "Scene: Night" << endl; }
      if (now3 && !prev3) { sceneManager.setScene(SceneManager::SUNSET); cout << "Scene: Sunset" << endl; }
      prev1 = now1; prev2 = now2; prev3 = now3;
    }

    // Camera mode switching: O, I, U
    { static bool prevO = false, prevI = false, prevU = false;
      bool nowO = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
      bool nowI = glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS;
      bool nowU = glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS;
      if (nowO && !prevO) camera.setMode(CameraMode::ORBIT);
      if (nowI && !prevI) camera.setMode(CameraMode::STATIC_FRONT);
      if (nowU && !prevU) camera.setMode(CameraMode::FREE);
      prevO = nowO; prevI = nowI; prevU = nowU;
    }

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
          armWave = false;
          headBob = false;
          torsoSway = false;
          cout << "Reset: All animations stopped" << endl;
      }
      prev = now;
    }

    // W: toggle arm wave animation
    { static bool prev = false;
      bool now = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
      if (now && !prev) { armWave = !armWave; cout << "Arm Wave: " << (armWave ? "ON" : "OFF") << endl; }
      prev = now;
    }

    // B: toggle head bobbing animation
    { static bool prev = false;
      bool now = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
      if (now && !prev) { headBob = !headBob; cout << "Head Bob: " << (headBob ? "ON" : "OFF") << endl; }
      prev = now;
    }

    // T: toggle torso rotation animation
    { static bool prev = false;
      bool now = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
      if (now && !prev) { torsoSway = !torsoSway; cout << "Torso Sway: " << (torsoSway ? "ON" : "OFF") << endl; }
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

    // --- arm wave animation ---
    if (armWave) {
        float t = (float)glfwGetTime();
        float leftArm = 60.0f * sinf(1.5f * t);      // Wave up and down
        float rightArm = 60.0f * sinf(1.5f * t + 3.14159f);  // Opposite phase
        setArms(leftArm, rightArm);
    } else {
        setArms(0.0f, 0.0f);  // Reset to neutral
    }

    // --- head bobbing animation ---
    if (headBob) {
        float t = (float)glfwGetTime();
        float headAngle = 15.0f * sinf(2.5f * t);    // Gentle nod
        setHead(headAngle);
    } else {
        setHead(0.0f);  // Reset to neutral
    }

    // --- torso sway animation ---
    if (torsoSway) {
        float t = (float)glfwGetTime();
        float torsoAngle = 10.0f * sinf(1.0f * t);   // Slow gentle sway
        setTorsoRotation(torsoAngle);
    } else {
        setTorsoRotation(0.0f);  // Reset to neutral
    }

    // --- apply the leg pose once per frame ---
   setLeftLeg(hipL_Ang, kneeL_Ang);

    // --- Get current scene ---
    const Scene& currentScene = sceneManager.getCurrentScene();

    // --- Update camera ---
    camera.update(window, deltaTime);
    glm::mat4 view = camera.getViewMatrix();
    glm::vec3 camPos = camera.getPosition();

    // --- Update uniforms with scene properties and camera ---
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, glm::value_ptr(camPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(currentScene.lightPosition));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightCol"), 1, glm::value_ptr(currentScene.lightColor));

    // --- draw ---
    glClearColor(currentScene.backgroundColor.r, currentScene.backgroundColor.g, currentScene.backgroundColor.b, 1.0f);
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

