#include "SocialMedia.h"
#include <iostream>
#include "lss.h"

void SocialMedia::MainFeed(float position, float width, float height)
{
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Main Feed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::GetStyle().ChildBorderSize = 0.0f;

    ImGui::BeginChild("Feed", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
    for (int i = 0; i < 10; ++i) {
        Lss::Image();

        ImGui::Separator();
    }
    ImGui::EndChild();

    ImGui::End();
}
void SocialMedia::LeftSide(float position, float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Left Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    float originalFontSize = ImGui::GetFont()->FontSize;

    Lss::Text("ColorSync", 5 * Lss::VH, Centered);

    ImGui::Separator();
    Lss::Top(2 * Lss::VH);
    Lss::Button("Heooo", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded);
    Lss::Back();
    Lss::End();
    ImGui::End();
}
void SocialMedia::RightSide(float position, float width, float height)
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