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
    static void ColorWindow(RenderData& cursor);
    static void SizeWindow(float& cursorRadius);
    static void BrushWindow(GLFWwindow* window);

    static void ServerWindow();
    static void LayerWindow();
    static void ChatWindow();

    static void LoginWindow();

    static void InitData(std::string usernameIn, std::string tokenIn);

    static void GetMsg(std::map<std::string, std::string> messageObject);

    static std::string GetUsername();
    static std::string GetToken();

    static void SetRenderer(NewRenderer& renderer);
};