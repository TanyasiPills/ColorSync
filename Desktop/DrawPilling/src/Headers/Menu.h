#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

class Menu {
private:
public:
	static void MainFeed(float position, float width, float height);
	static void LeftSide(float position, float width, float height);
	static void RightSide(float position, float width, float height);
};