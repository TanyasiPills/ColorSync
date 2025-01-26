#include "DrawUI.h"

ImVec2 ColorWindowSize(100, 200);
ImVec2 ColorWindowPos;
ImVec2 SizeWindowSize(100, 200);
ImVec2 SizeWindowPos(0, 200);
ImVec2 BrushWindowSize(100, 200);
ImVec2 BrushWindowPos(0, 450);

ImVec2 ServerWindowSize(100, 200);
ImVec2 ServerWindowPos;
ImVec2 LayerWindowSize(100, 200);
ImVec2 LayerWindowPos(100, 200);
int leftSize = 200;
int rightSize = 200;

int windowSizeX, windowSizeY;

void DrawUI::ColorWindow(RenderData& cursor)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftSize, SizeWindowPos.y));

	ImGui::Begin("Color", nullptr, ImGuiWindowFlags_NoTitleBar);
	static float color[3] = { 0.0f, 0.0f, 0.0f };
	ImGui::ColorEdit3("##c", color, ImGuiColorEditFlags_NoSidePreview);
	ImGui::ColorPicker3("##MyColor##6", (float*)&color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
	cursor.shader->SetUniform3f("Kolor", color[0], color[1], color[2]);

	ColorWindowSize = ImGui::GetWindowSize();
	leftSize = ColorWindowSize.x;
	ColorWindowPos = ImGui::GetWindowPos();
	ImGui::End();
}

void DrawUI::SizeWindow(float& cursorRadius)
{
	ImGui::SetNextWindowPos(ImVec2(0, ColorWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftSize, BrushWindowPos.y - SizeWindowPos.y));

	ImGui::Begin("Size", nullptr, ImGuiWindowFlags_NoTitleBar);

	static char selected[4][4] = { { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++)
		{
			int index = y * 4 + x + 1;
			//int indexText = std::pow(2, (float)index);
			std::string label = std::to_string(index);

			if (x > 0) ImGui::SameLine();

			ImGui::PushID(y * 4 + x);
			if (ImGui::Selectable(label.c_str(), selected[y][x] != 0, 0, ImVec2(50, 50)))
			{
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						selected[i][j] = 0;
					}
				}
				cursorRadius = 0.01 * index;
				selected[y][x] ^= 1;
			}
			ImGui::PopID();
		}
	}
	SizeWindowSize = ImGui::GetWindowSize();
	leftSize = SizeWindowSize.x;
	SizeWindowPos = ImGui::GetWindowPos();
	ImGui::End();
}

void DrawUI::BrushWindow(GLFWwindow* window) 
{
	glfwGetWindowSize(window, &windowSizeX, &windowSizeY);
	ImGui::SetNextWindowPos(ImVec2(0, SizeWindowPos.y + SizeWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftSize, windowSizeY - (SizeWindowPos.y + SizeWindowSize.y)));

	ImGui::Begin("Brushes", nullptr, ImGuiWindowFlags_NoTitleBar);

	static char selectedBrush[4][4] = { { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++)
		{
			int index = y * 4 + x + 1;
			std::string label = "Bruh " + std::to_string(index);

			if (x > 0) ImGui::SameLine();

			ImGui::PushID(y * 4 + x);
			if (ImGui::Selectable(label.c_str(), selectedBrush[y][x] != 0, 0, ImVec2(50, 50)))
			{
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						selectedBrush[i][j] = 0;
					}
				}
				selectedBrush[y][x] ^= 1;
			}
			ImGui::PopID();
		}
	}
	BrushWindowSize = ImGui::GetWindowSize();
	leftSize = BrushWindowSize.x;
	BrushWindowPos = ImGui::GetWindowPos();
	ImGui::End();
}

void DrawUI::ServerWindow() 
{
	ImGui::SetNextWindowPos(ImVec2(windowSizeY-rightSize, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, LayerWindowPos.y));

	ImGui::Begin("Lobby", nullptr, ImGuiWindowFlags_NoTitleBar);
	ServerWindowSize = ImGui::GetWindowSize();
	rightSize = ServerWindowSize.x;
	ServerWindowPos = ImGui::GetWindowPos();
	ImGui::End();
}