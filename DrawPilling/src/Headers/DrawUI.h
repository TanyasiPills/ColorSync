#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "NewRenderer.h"

class DrawUI
{
private:

public:
    static void ColorWindow(RenderData& cursor);
    static void SizeWindow(float& cursorRadius);
    static void BrushWindow(GLFWwindow* window);

    static void ServerWindow();
    static void LayerWindow();
    static void ChatWindow();
};