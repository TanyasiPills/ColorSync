#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "NewRenderer.h"

struct Layer {
    std::string name;
    std::vector<Layer> children;
    bool visible = true;
    bool editing = false;

    Layer(const std::string& layerName) : name(layerName) {}
};

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