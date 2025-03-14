#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class ShaderFileReader {
public:
	static GLuint loadShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile) {
        // Read vertex shader source code
        std::ifstream vertexShaderStream(vertexShaderFile);
        if (!vertexShaderStream.is_open()) {
            std::cerr << "Failed to open vertex shader file: " << vertexShaderFile << std::endl;
            return 0;
        }
        std::stringstream vertexShaderBuffer;
        vertexShaderBuffer << vertexShaderStream.rdbuf();
        std::string vertexShaderSource = vertexShaderBuffer.str();
        const char* vertexShaderSourcePtr = vertexShaderSource.c_str();
        vertexShaderStream.close();

        // Read fragment shader source code
        std::ifstream fragmentShaderStream(fragmentShaderFile);
        if (!fragmentShaderStream.is_open()) {
            std::cerr << "Failed to open fragment shader file: " << fragmentShaderFile << std::endl;
            return 0;
        }
        std::stringstream fragmentShaderBuffer;
        fragmentShaderBuffer << fragmentShaderStream.rdbuf();
        std::string fragmentShaderSource = fragmentShaderBuffer.str();
        const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();
        fragmentShaderStream.close();

        // Compile vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, NULL);
        glCompileShader(vertexShader);
        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << "Failed to compile vertex shader: " << infoLog << std::endl;
            return 0;
        }

        // Compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "Failed to compile fragment shader: " << infoLog << std::endl;
            return 0;
        }

        // Link shaders into shader program
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "Failed to link shader program: " << infoLog << std::endl;
            return 0;
        }

        // Clean up
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }
};