#include "robot.h"
#include <glm/gtc/matrix_transform.hpp>

using glm::mat4;
using glm::vec3;

// Joint state
struct RobotJoints {
    float neck = 0.0f;
    float shoulderL = 0.0f, elbowL = 0.0f;
    float shoulderR = 0.0f, elbowR = 0.0f;
    float hipL = 0.0f, kneeL = 0.0f;
    float hipR = 0.0f, kneeR = 0.0f;
    float torsoRotation = 0.0f;
};
static RobotJoints gJ;

// Matrix helpers
static mat4 I()                      { return mat4(1.0f); }
static mat4 T(const vec3& t)         { return glm::translate(I(), t); }
static mat4 S(const vec3& s)         { return glm::scale(I(), s); }
static mat4 Rx(float deg)            { return glm::rotate(I(), glm::radians(deg), vec3(1,0,0)); }
static mat4 Ry(float deg)            { return glm::rotate(I(), glm::radians(deg), vec3(0,1,0)); }
static mat4 Rz(float deg)            { return glm::rotate(I(), glm::radians(deg), vec3(0,0,1)); }

static void setMat4(GLuint program, const char* name, const mat4& M) {
    GLint loc = glGetUniformLocation(program, name);
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, &M[0][0]);
}
static void setColor(GLuint program, const vec3& c) {
    GLint loc = glGetUniformLocation(program, "objectCol");
    if (loc >= 0) glUniform3fv(loc, 1, &c[0]);
}

static void drawCube(GLuint program,
                     GLuint cubeVAO,
                     const mat4& model,
                     const mat4& view,
                     const mat4& proj,
                     const vec3& color)
{
    setMat4(program, "model", model);
    setMat4(program, "view",  view);
    setMat4(program, "projection",  proj);
    setColor(program, color);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Robot body part sizes
static const vec3 TORSO = {1.0f, 1.6f, 0.5f};
static const vec3 HEAD  = {0.5f, 0.5f, 0.5f};
static const vec3 UARM  = {0.35f, 0.9f, 0.35f};
static const vec3 FARM  = {0.30f, 0.9f, 0.30f};
static const vec3 THIGH = {0.45f, 1.0f, 0.45f};
static const vec3 SHIN  = {0.40f, 1.0f, 0.40f};

void updateJointsFromInput(GLFWwindow* window, float dt)
{
    const float s = 60.0f * dt;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) gJ.neck    += s;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) gJ.neck    -= s;

    // Arms
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) gJ.shoulderL += s;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) gJ.shoulderL -= s;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) gJ.elbowL    += s;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) gJ.elbowL    -= s;

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) gJ.shoulderR += s;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) gJ.shoulderR -= s;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) gJ.elbowR    += s;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) gJ.elbowR    -= s;

    // Legs
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) gJ.hipL  += s;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) gJ.hipL  -= s;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) gJ.kneeL += s;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) gJ.kneeL -= s;

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) gJ.hipR  += s;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) gJ.hipR  -= s;
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) gJ.kneeR += s;
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) gJ.kneeR -= s;
}

void drawRobot(GLuint program,
               GLuint cubeVAO,
               const mat4& view,
               const mat4& proj)
{
    mat4 torsoBase = T({0, 1.0f, 0}) * Ry(gJ.torsoRotation);

    // Torso
    mat4 torsoM = torsoBase * S(TORSO);
    drawCube(program, cubeVAO, torsoM, view, proj, {0.75f, 0.75f, 0.85f});

    // Head
    mat4 neckBase = torsoBase * T({0, TORSO.y * 0.5f, 0});
    mat4 headM = neckBase * Ry(gJ.neck) * T({0, HEAD.y * 0.5f, 0}) * S(HEAD);
    drawCube(program, cubeVAO, headM, view, proj, {0.9f, 0.8f, 0.7f});

    // Arms
    const float shoulderY = TORSO.y * 0.35f;
    const float shoulderX = (TORSO.x * 0.5f) + (UARM.x * 0.5f) * 0.9f;

    // Left arm
    {
        mat4 shoulder = torsoBase * T({-shoulderX, shoulderY, 0});
        mat4 upper = shoulder
                   * Rz(gJ.shoulderL)
                   * T({0, -UARM.y * 0.5f, 0})
                   * S(UARM);
        drawCube(program, cubeVAO, upper, view, proj, {0.8f, 0.3f, 0.3f});

        mat4 elbowBase = shoulder * Rz(gJ.shoulderL) * T({0, -UARM.y, 0});
        mat4 fore = elbowBase
                  * Rz(gJ.elbowL)
                  * T({0, -FARM.y * 0.5f, 0})
                  * S(FARM);
        drawCube(program, cubeVAO, fore, view, proj, {0.85f, 0.4f, 0.4f});
    }

    // Right arm
    {
        mat4 shoulder = torsoBase * T({+shoulderX, shoulderY, 0});
        mat4 upper = shoulder
                   * Rz(-gJ.shoulderR)
                   * T({0, -UARM.y * 0.5f, 0})
                   * S(UARM);
        drawCube(program, cubeVAO, upper, view, proj, {0.3f, 0.3f, 0.8f});

        mat4 elbowBase = shoulder * Rz(-gJ.shoulderR) * T({0, -UARM.y, 0});
        mat4 fore = elbowBase
                  * Rz(-gJ.elbowR)
                  * T({0, -FARM.y * 0.5f, 0})
                  * S(FARM);
        drawCube(program, cubeVAO, fore, view, proj, {0.4f, 0.4f, 0.85f});
    }

    // Legs
    const float hipY = 1.0f - TORSO.y * 0.5f;
    const float hipX = TORSO.x * 0.3f;

    // Left leg
    {
        mat4 hip = T({-hipX, hipY, 0});
        mat4 thigh = hip
                   * Rx(gJ.hipL)
                   * T({0, -THIGH.y * 0.5f, 0})
                   * S(THIGH);
        drawCube(program, cubeVAO, thigh, view, proj, {0.3f, 0.7f, 0.3f});

        mat4 kneeBase = hip * Rx(gJ.hipL) * T({0, -THIGH.y, 0});
        mat4 shin = kneeBase
                  * Rx(gJ.kneeL)
                  * T({0, -SHIN.y * 0.5f, 0})
                  * S(SHIN);
        drawCube(program, cubeVAO, shin, view, proj, {0.35f, 0.8f, 0.35f});
    }

    // Right leg
    {
        mat4 hip = T({+hipX, hipY, 0});
        mat4 thigh = hip
                   * Rz(-gJ.hipR)
                   * T({0, -THIGH.y * 0.5f, 0})
                   * S(THIGH);
        drawCube(program, cubeVAO, thigh, view, proj, {0.2f, 0.65f, 0.2f});

        mat4 kneeBase = hip * Rz(-gJ.hipR) * T({0, -THIGH.y, 0});
        mat4 shin = kneeBase
                  * Rz(-gJ.kneeR)
                  * T({0, -SHIN.y * 0.5f, 0})
                  * S(SHIN);
        drawCube(program, cubeVAO, shin, view, proj, {0.25f, 0.7f, 0.25f});
    }
}

void setLeftLeg(float hipDeg, float kneeDeg) {
    gJ.hipL  = hipDeg;
    gJ.kneeL = kneeDeg;
}

void setArms(float leftShoulderDeg, float rightShoulderDeg) {
    gJ.shoulderL = leftShoulderDeg;
    gJ.shoulderR = rightShoulderDeg;
}

void setHead(float neckDeg) {
    gJ.neck = neckDeg;
}

void setTorsoRotation(float rotationDeg) {
    gJ.torsoRotation = rotationDeg;
}
