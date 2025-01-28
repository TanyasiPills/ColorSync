#pragma once
#include <iostream>

struct ShaderSource {
    std::string Vertex;
    std::string Fragment;
};
struct ShaderAndLocs
{
    unsigned int shader;
    GLint colorLoc;
    GLint xRatio;
    GLint yRatio;
    GLint xOffset;
    GLint yOffset;
    GLint scale;
};

class Shadering {
public:
	static ShaderSource ParseShader(const std::string& filepath);
    static unsigned int ComplileShader(const std::string& source, unsigned int type);
    static ShaderAndLocs CreateShader(unsigned int& VBO, unsigned int& VAO);
};