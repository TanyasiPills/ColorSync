#pragma once

class Renderer {
public:
	static void RenderScreen(GLFWwindow* window, unsigned int& VBO, unsigned int& VAO, float* size, bool* hover, unsigned int& canvasVBO, unsigned int& canvasVAO);
	static void RenderUI(GLint& colorLoc, float* size, bool* hover);
	static float* GetColorPointer();
};