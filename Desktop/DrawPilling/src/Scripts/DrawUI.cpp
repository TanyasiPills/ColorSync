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
ImVec2 LayerWindowPos(0, 200);
ImVec2 ChatWindowSize(100, 200);
ImVec2 ChatWindowPos(0, 450);
int leftSize = 200;
int rightSize = 200;

int windowSizeX, windowSizeY;

std::string username = "Maychii";

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
	ImGui::SetNextWindowPos(ImVec2(windowSizeX-rightSize, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, LayerWindowPos.y));

	ImGui::Begin("Lobby", nullptr, ImGuiWindowFlags_NoTitleBar);
	ServerWindowSize = ImGui::GetWindowSize();
	rightSize = ServerWindowSize.x;
	ServerWindowPos = ImGui::GetWindowPos();
	ImGui::End();
}

void DrawLayerTree(Layer& layer) {
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
	layer.open = ImGui::TreeNodeEx(("##" + layer.name).c_str(), ImGuiTreeNodeFlags_FramePadding | (layer.open ? ImGuiTreeNodeFlags_DefaultOpen : 0));
	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 10));
	ImGui::Checkbox(("##" + layer.name + "visibility").c_str(), &layer.visible);
	ImGui::SameLine();
	
	if(layer.visible && !layer.editing) ImGui::Text((layer.name).c_str());

	if (layer.visible && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
		layer.editing = true;
	}

	ImGui::PopStyleVar();

	if (layer.visible && layer.editing) {
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		static char editBuffer[256] = "";
		bool editing = ImGui::InputText("##ChatInput", editBuffer, IM_ARRAYSIZE(editBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SetKeyboardFocusHere(-1);
		if (editing) {
			if (strlen(editBuffer) > 0) {
				layer.name = editBuffer;
				memset(editBuffer, 0, sizeof(editBuffer));
				layer.editing = false;
			}
		}
	}

	if (layer.open) {
		for (auto& child : layer.children) {
			DrawLayerTree(child);
		}

		ImGui::TreePop();
	}
	ImGui::PopStyleVar();
}

void DrawUI::LayerWindow()
{
	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, ServerWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, ChatWindowPos.y - LayerWindowPos.y));

	static Layer rootLayer("Root Layer");

	if (rootLayer.children.empty()) {
		rootLayer.children.push_back(Layer("Background"));
		rootLayer.children.push_back(Layer("Foreground"));
		rootLayer.children[1].children.push_back(Layer("Sub-layer 1"));
		rootLayer.children[1].children.push_back(Layer("Sub-layer 2"));
	}

	ImGui::Begin("Layer", nullptr, ImGuiWindowFlags_NoTitleBar);

	DrawLayerTree(rootLayer);

	LayerWindowSize = ImGui::GetWindowSize();
	rightSize = LayerWindowSize.x;
	LayerWindowPos = ImGui::GetWindowPos();
	ImGui::End();
}

void DrawUI::ChatWindow()
{
	static std::vector<std::string> chatLog;
	static char inputBuffer[256] = "";

	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, LayerWindowPos.y + LayerWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, windowSizeY - (LayerWindowPos.y + LayerWindowSize.y)));

	ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoTitleBar);

	float inputBarHeight = ImGui::GetTextLineHeight() * 2 + ImGui::GetStyle().FramePadding.y * 3 + ImGui::GetStyle().ItemSpacing.y;

	ImGui::BeginChild("ChatLog", ImVec2(0, ChatWindowSize.y - inputBarHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& message : chatLog)
	{
		ImGui::TextWrapped("%s: %s", username.c_str(), message.c_str());
	}
	ImGui::EndChild();
	ImGui::Separator();
	ImGui::BeginChild("ChatInput", ImVec2(0, inputBarHeight), false);
	if (ImGui::InputText("##ChatInput", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (strlen(inputBuffer) > 0)
		{
			chatLog.push_back(inputBuffer);
			memset(inputBuffer, 0, sizeof(inputBuffer)); 
		}
		ImGui::SetKeyboardFocusHere(-1);
	}
	ImGui::EndChild();

	ChatWindowSize = ImGui::GetWindowSize();
	rightSize = ChatWindowSize.x;
	ChatWindowPos = ImGui::GetWindowPos();
	ImGui::End();
}
