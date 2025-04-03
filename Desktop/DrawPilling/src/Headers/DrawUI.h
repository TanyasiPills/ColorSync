#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "NewRenderer.h"

#include <map>
#include <string>

class DrawUI
{
private:

public:
    static void DrawMenu();

    static void ColorWindow(RenderData& cursor);
    static void SizeWindow(float& cursorRadius, float scale = 1.0f);
    static void BrushWindow(GLFWwindow* window, RenderData& cursor);

    static void ServerWindow();
    static void LayerWindow();
    static void ChatWindow();

    static void InitWindow();
    static void InitData();

    static void GetMsg(std::map<std::string, std::string> messageObject);
    static void PlayerVisualization();

    static void SetColor(float* colorIn);
    static void SetRenderer(NewRenderer& renderer);

    static void DeleteChilds(int& index);
};