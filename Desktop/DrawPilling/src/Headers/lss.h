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
	Rounded = 1 << 2,
	Trans = 1 << 3,
	SameLine = 1 << 4
};

enum Colors {
	Background = 0,
	ContainerBackground = 1,
	LowHighlight = 2,
	HeavyHighlight = 3,
	Border = 4,
	Font = 5,
	Transparent = 6
};

class Lss {
private:
public:
	static float VH;
	static float VW;

	static void Init(GLFWwindow* windowIn, int screenWidth, int screenHeight);
	static void End(); // End of frame

	static bool InBound(ImVec2 pos, ImVec2 start, ImVec2 size);

	static void SetFontSize(float size);
	static void SetColor(int region, int colorToSet);

	static void Text(std::string textIn, float size, int flags = None);
	static bool Button(std::string textIn, ImVec2 size, float textSizeIn, int flags = None);
	static void Image(GLuint texture = 69, ImVec2 size = ImVec2(-1, -1), int flags = None);
	static void Child(std::string name, ImVec2 size = ImVec2(0,0), bool border = true, 
		int flags = None, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None);
	static bool InputText(std::string label, char* buffer, size_t buffer_size, ImVec2 size = ImVec2(10*VH, 2*VH), int flags = None, int inputFlags = ImGuiInputFlags_None, int maxWidth = 0);
	static bool InputInt(std::string label, int* value, ImVec2 size = ImVec2(10 * VH, 2 * VH), int flags = None, int inputFlags = ImGuiInputFlags_None);
	static bool Modal(std::string label, bool* open, ImVec2 size = ImVec2(0,0), int flags = 0, int windowFlags = 0);
	static void Separator(float thickness = 1.0f, float width = 0.0f, int color = 4, int flags = 0);

	static void Left(float distance);
	static void Right(float distance);
	static void Top(float disctance);
	static void Bottom(float distance);

	static void LeftTop(float leftIn, float topIn);
	static void LeftBottom(float leftIn, float bottomIn);
	static void RightTop(float rightIn, float topIn);
	static void RightBottom(float rightIn, float bottomIn);

	static void Back(); // reset cursor pos



};