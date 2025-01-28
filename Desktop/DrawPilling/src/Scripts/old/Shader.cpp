#include <fstream>
#include <sstream>
#include <iostream>
#include "GLEW/glew.h"
#include <GLFW/glfw3.h> 
#include "old/Shader.h"
#include <old/GLManager.h>

ShaderSource Shadering::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shadering::ComplileShader(const std::string& source, unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)alloca(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

ShaderAndLocs Shadering::CreateShader(unsigned int& VBO, unsigned int& VAO)
{
    GLManager::initBuffers(VBO, VAO);

    ShaderSource source = Shadering::ParseShader("Resources/Shaders/Style.shader");

    unsigned int program = glCreateProgram();
    unsigned int vertShader = Shadering::ComplileShader(source.Vertex, GL_VERTEX_SHADER);
    unsigned int fragShader = Shadering::ComplileShader(source.Fragment, GL_FRAGMENT_SHADER);

    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    GLint colorLocation = glGetUniformLocation(program, "circleColor");
    GLint xRation = glGetUniformLocation(program, "xRatio");
    GLint yRation = glGetUniformLocation(program, "yRatio");
    GLint xOffsetet = glGetUniformLocation(program, "xOffset");
    GLint yOffsetet = glGetUniformLocation(program, "yOffset");
    GLint scalel = glGetUniformLocation(program, "scale");

    ShaderAndLocs locs;
    locs.colorLoc = colorLocation;
    locs.shader = program;
    locs.xRatio = xRation;
    locs.yRatio = yRation;
    locs.xOffset = xOffsetet;
    locs.yOffset = yOffsetet;
    locs.scale = scalel;

    glUseProgram(program);

    return locs;
}