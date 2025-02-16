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

enum Colors {
	Background = 0,
	ContainerBackground = 1,
	LowHighlight = 2,
	HeavyHighlight = 3,
	Border = 4,
	Font = 5,
};

class Lss {
private:
public:
	static float VH;
	static float VW;

	static void Init(GLFWwindow* windowIn, int screenWidth, int screenHeight);
	static void End(); // End of frame


	static void SetFontSize(float size);
	static void SetColor(int region, int colorToSet);

	static void Text(std::string textIn, float size, int flags = None);
	static bool Button(std::string textIn, ImVec2 size, float textSizeIn, int flags = None);
	static void Image(GLuint texture = 69, ImVec2 size = ImVec2(-1, -1), int flags = None);
	static void Child(std::string name, ImVec2 size = ImVec2(0,0), bool border = true, 
		int flags = None, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None);


	static void Left(float distance);
	static void Top(float disctance);
	static void Bottom(float distance);
	static void Back();



};