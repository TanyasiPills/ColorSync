#include <DrawMenu.h>

void Menu::MainFeed()
{
    ImGui::Begin("Main Feed", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    // Create the main feed in the center, scrollable
    ImGui::BeginChild("Feed", ImVec2(0, -50), true); // Allow vertical scrolling, no header
    for (int i = 0; i < 10; ++i) {
        ImGui::Text("Post %d", i + 1);
        // Add more content here (e.g., images, likes, comments, etc.)
        ImGui::Separator();
    }
    ImGui::EndChild();

    ImGui::End();
}
void Menu::LeftSide()
{
    ImGui::Begin("Left Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    // Add buttons or other stuff to the left side window
    if (ImGui::Button("Button 1")) {
        // Handle button click
    }
    if (ImGui::Button("Button 2")) {
        // Handle button click
    }

    ImGui::End();
}
void Menu::RigtSide()
{
    ImGui::Begin("Right Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::Button("Button A")) {
    }
    if (ImGui::Button("Button B")) {
    }

    ImGui::End();
}