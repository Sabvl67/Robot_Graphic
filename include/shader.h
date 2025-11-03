#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

GLuint compileShader(const char* shaderSource, GLenum shaderType);

GLuint CreateShaderProgram(GLuint vertexShader, GLuint fragmentShader);

GLuint loadShaderProgram (const char* vertexPath, const char* fragmentPath);

#endif