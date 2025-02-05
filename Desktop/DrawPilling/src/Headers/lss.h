#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "GLEW/glew.h"
#include <GLFW/glfw3.h> 
#include <string>


class Lss {
private:
public:
	static float VH;
	static float VW;

	static void Init(GLFWwindow* windowIn);
	static void Update();

	static void Text(std::string textIn, float size, bool centeredIn = false);
	static void Button();
};