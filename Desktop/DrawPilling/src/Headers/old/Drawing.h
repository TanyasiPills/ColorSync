#pragma once

#include <vector>
#include "GLEW/glew.h"
#include <GLFW/glfw3.h>
#include "NCallBacks.h"

class Drawing {
public:
	static std::vector<float> drawCircle(float& x, float& y, float& r, int& sides);
	static void handleCursorMovement(GLFWwindow* window, double& prevXpos, double& prevYpos, std::vector<std::vector<float>>& circles, GLuint VBO, GLuint VAO, float radius, int sides, bool hover);
	static void handleCanvasMovement(GLFWwindow* window, unsigned int& VBO, unsigned int& VAO);
	static void initDrawData(Ratios& ratios);
};