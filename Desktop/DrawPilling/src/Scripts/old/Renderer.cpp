#define GLEW_STATIC
#include "GLEW/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> 

#include "old/GLManager.h"
#include "old/Drawing.h"
#include "old/Renderer.h"


std::vector<std::vector<float>> circles;
std::vector<float> prevCircles;
unsigned int canvasVBO;

static float color[3] = { 0.0f, 0.0f, 1.0f };

ImVec4 clear_color = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);

void Renderer::RenderScreen(GLFWwindow* window, unsigned int& VBO, unsigned int& VAO, float* size, bool* hover, unsigned int& canvasVBO, unsigned int& canvasVAO) {
    ImGui::Render();
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    Drawing::handleCanvasMovement(window,canvasVBO,canvasVAO);

    for (const auto& circle : circles) {
        float color[3] = { circle[0], circle[1], circle[2] };
        std::vector<float> verticesToDraw(circle.begin() + 3, circle.end());
        GLManager::updateVBO(VBO, verticesToDraw);
        GLManager::drawStuff(VBO, GL_TRIANGLE_FAN, verticesToDraw, color);
    }

    static double prevXpos, prevYpos;
    Drawing::handleCursorMovement(window, prevXpos, prevYpos, circles, VBO, VAO, *size, 24, *hover);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void Renderer::RenderUI(GLint& colorLoc,float* size, bool* hover) {
    ImGui::Begin("Controls", 0);

    float label_width = 200.0f + ImGui::GetStyle().ItemSpacing.x;
    ImGui::PushItemWidth(label_width);
    ImVec2 content_size = ImGui::GetWindowSize();
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);

    ImGui::DragFloat("##x", size, 0.01f, 0.0f, 0.0f, "%.04f");
    ImGui::SeparatorText("Color");
    ImGui::ColorEdit3("##c", color);

    *hover = ImGui::IsWindowHovered();

    ImGui::PopItemWidth();

    ImGui::End();
}

float* Renderer::GetColorPointer() {
    return color;
}