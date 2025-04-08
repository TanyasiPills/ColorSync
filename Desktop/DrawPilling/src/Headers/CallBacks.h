#pragma once
#include "GLEW/glew.h"
#include <GLFW/glfw3.h> 
#include "NewRenderer.h"

class Callback
{
private:
	
public:
	static void Init(GLFWwindow* window, NewRenderer& rendererIn);
	static double* GlCursorPosition();
	static void EditorSwapCallBack(bool online = false);
};