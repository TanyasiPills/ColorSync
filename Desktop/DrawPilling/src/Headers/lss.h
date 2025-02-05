#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "GLEW/glew.h"
#include <GLFW/glfw3.h> 
#include <string>

enum CSS {
	None = 0,
	Centered = 1 << 0,
	Invisible = 1 << 1, 
	Rounded = 1 << 2
};

class Lss {
private:
public:
	static float VH;
	static float VW;

	static void Init(GLFWwindow* windowIn, int screenWidth, int screenHeight);
	static void Update();
	static void SetFontSize(float size);

	static void Text(std::string textIn, float size, int flags = None);
	static void Button(std::string textIn, ImVec2 size, float textSizeIn, int flags = None);

	static void End();
};