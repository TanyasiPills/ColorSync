#include "Menu.h"
#include <iostream>

void Menu::MainFeed(float position, float width, float height)
{
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Main Feed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    ImGui::BeginChild("Feed", ImVec2(0, 0), true);
    for (int i = 0; i < 10; ++i) {
        ImGui::Text("Post %d", i + 1);
        ImGui::Separator();
    }
    ImGui::EndChild();

    ImGui::End();
}
void Menu::LeftSide(float position, float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Left Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    
    float originalFontSize = ImGui::GetFont()->FontSize;

    ImGui::GetFont()->FontSize = 200.0f;

    float textWidth = ImGui::CalcTextSize("ColorSync").x;
    ImGui::SetCursorPosX((width - textWidth) * 0.5f);
    ImGui::Text("ColorSync");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);

    ImGui::BeginChild("Editor", ImVec2(0, 100), true);

    ImGui::EndChild();
    ImGui::BeginChild("Lobbies", ImVec2(0, 100), true);
    ImGui::EndChild();

    // Restore the original font size
    ImGui::GetFont()->FontSize = originalFontSize;

    ImVec4 originalBtnBgColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
    ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
    float originalRounding = ImGui::GetStyle().FrameRounding;
    ImGui::GetStyle().FrameRounding = 2.0f;
    ImVec2 buttonSize(200.0f, 50.0f);
    // Add buttons or other stuff to the left side window
    if (ImGui::Button("Button 1", buttonSize)) {
        // Handle button click
    }
    if (ImGui::Button("Button 2", buttonSize)) {
        // Handle button click
    }
    ImGui::GetStyle().Colors[ImGuiCol_Button] = originalBtnBgColor;
    ImGui::GetStyle().FrameRounding = originalRounding;

    ImGui::End();
}
void Menu::RightSide(float position, float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Right Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::Button("Button A")) {
    }
    if (ImGui::Button("Button B")) {
    }

    ImGui::End();
}