#pragma once
#include "Shader.h"

struct Ratios {
	float* xRatio;
	float* yRatio;
	float* xOffset;
	float* yOffset;
	float* scale;
};

class CallBackManager {
public:
	 static Ratios SetCallBacks(GLFWwindow* window, unsigned int* VBO, unsigned int* VAO, float* size, bool* hover, ShaderAndLocs& shaderAndLocs, unsigned int* canvasVBO, unsigned int* canvasVAO);
	 static void ProcessInput(GLFWwindow* window);
};