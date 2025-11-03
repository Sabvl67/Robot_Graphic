#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

// read shader files
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