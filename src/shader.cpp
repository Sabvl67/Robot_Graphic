#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

string loadShader(const char* filePath) {
    string shaderCode;
    ifstream shaderFile;

    shaderFile.exceptions(ifstream::failbit| ifstream::badbit);
    try {
        shaderFile.open(filePath);
        stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch (ifstream::failure& e){
        cerr << "Error: Fail to read shader" << filePath << "/n";
    }
    return shaderCode;
}

GLuint compileShader(const char* shaderSource, GLenum shaderType){
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cerr << "Error: Shader compilation failed " << "\n" << infoLog << endl;
    }
    return shader;
}

GLuint CreateShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        cerr << "Error: Shader program linking failed" << "\n" << infoLog << "\n";
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint loadShaderProgram(const char* vertexPath, const char* fragmentPath){
    string vertexCode = loadShader(vertexPath);
    string fragmentCode = loadShader(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = compileShader(vShaderCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fShaderCode, GL_FRAGMENT_SHADER);

    GLuint program = CreateShaderProgram(vertexShader, fragmentShader);

    cout << "Shader completed" << endl;

    return program;
}