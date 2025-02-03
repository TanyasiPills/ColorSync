#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "GLEW/glew.h"
#include <GLFW/glfw3.h> 


class Lss {
private:
public:
	static void Init(GLFWwindow* windowIn);
	static void Update();

	static int VW();
	static int VH();
};