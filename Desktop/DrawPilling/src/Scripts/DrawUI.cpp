#include "DrawUI.h"
#include "HighsManager.h"
#include "SocksManager.h"
#include "RuntimeData.h"
#include "FileExplorer.h"
#include "CallBacks.h"
#include "lss.h"
#include <thread>

//Left side
ImVec2 ColorWindowSize(100, 200);
ImVec2 ColorWindowPos;
ImVec2 SizeWindowSize(100, 200);
ImVec2 SizeWindowPos(0, 200);
ImVec2 BrushWindowSize(100, 200);
ImVec2 BrushWindowPos(0, 450);
int leftSize = 200;
int leftMinSize = 200;

//Right side
ImVec2 ServerWindowSize(100, 200);
ImVec2 ServerWindowPos;
ImVec2 LayerWindowSize(100, 200);
ImVec2 LayerWindowPos(0, 200);
ImVec2 ChatWindowSize(100, 200);
ImVec2 ChatWindowPos(0, 450);
int rightSize = 400;
int rightMinSize = 400;


int windowSizeX, windowSizeY;
int selectedLayer = -1;

bool inited = false;
bool canvasInitWindow = true;
bool needLogin = true;
bool isOnline = false;

static std::vector<std::string> chatLog;

static NewRenderer* renderer;

static RuntimeData& runtime = RuntimeData::getInstance();

MyTexture visible;
MyTexture notVisible;
MyTexture folderLayer;
MyTexture layerLayer;

void DrawUI::SetRenderer(NewRenderer& rendererIn) {
	renderer = &rendererIn;
	isOnline = renderer->GetOnline();
	std::cout << "Is online: " << isOnline << std::endl;
}

void DrawUI::InitData()
{
	if (runtime.ip[0] == '\0') {
		std::cerr << "No ip in appdata" << std::endl;
	}
	if (runtime.username[0] == '\0') {
		std::cerr << "No username in appdata" << std::endl;
	}
	if (runtime.token[0] == '\0') {
		std::cerr << "No token in appdata" << std::endl;
	}
	if (runtime.password[0] == '\0') {
		std::cerr << "No password in appdata" << std::endl;
	}
	visible.Init("Resources/icons/eyesOpen.png");
	notVisible.Init("Resources/icons/eyeClosed.png");
	folderLayer.Init("Resources/icons/folderLayer.png");
	layerLayer.Init("Resources/icons/layer.png");
}

static float color[3] = { 0.0f, 0.0f, 0.0f };

void DrawUI::SetColor(float* colorIn)
{
	color[0] = colorIn[0];
	color[1] = colorIn[1];
	color[2] = colorIn[2];
}

void DrawUI::ColorWindow(RenderData& cursor)
{

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftSize, SizeWindowPos.y));

	ImGui::Begin("Color", nullptr, ImGuiWindowFlags_NoTitleBar | ((ColorWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	ImGui::SetNextItemWidth(-1);
	ImGui::ColorEdit3("##c", color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoLabel);
	ImGui::SetNextItemWidth(-1);
	ImGui::ColorPicker3("##MyColor##6", (float*)&color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
	if(renderer->inited)cursor.shader->SetUniform3f("Kolor", color[0], color[1], color[2]);
	renderer->SetColor(color);

	ColorWindowSize = ImGui::GetWindowSize();
	leftSize = ColorWindowSize.x;
	ColorWindowPos = ImGui::GetWindowPos();
	ImGui::End();
	if (ColorWindowSize.x < 200) {
		leftSize = 200;
		std::cout << leftSize << std::endl;
	}
}

void DrawUI::SizeWindow(float& cursorRadius)
{
	ImGui::SetNextWindowPos(ImVec2(0, ColorWindowSize.y), ImGuiCond_Always);

	ImGui::SetNextWindowSize(ImVec2(leftSize, BrushWindowPos.y - SizeWindowPos.y));

	ImGui::Begin("Size", nullptr, ImGuiWindowFlags_NoTitleBar | ((SizeWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));

	static char selected[4][4] = { { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	if (ImGui::SliderFloat("Size", &cursorRadius, 0.01f, 0.16f, "%.2f")) {
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x) {
				selected[y][x] = 0;
			}
		}
	}

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
	if (SizeWindowSize.x < 200) {
		leftSize = 200;
		std::cout << leftSize << std::endl;
	}
}

void DrawUI::BrushWindow(GLFWwindow* window) 
{
	glfwGetWindowSize(window, &windowSizeX, &windowSizeY);
	ImGui::SetNextWindowPos(ImVec2(0, SizeWindowPos.y + SizeWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftSize, windowSizeY - (SizeWindowPos.y + SizeWindowSize.y)));

	ImGui::Begin("Brushes", nullptr, ImGuiWindowFlags_NoTitleBar | ((BrushWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));

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
				renderer->tool = 4 * y + x;
			}
			ImGui::PopID();
		}
	}
	BrushWindowSize = ImGui::GetWindowSize();
	leftSize = BrushWindowSize.x;
	BrushWindowPos = ImGui::GetWindowPos();
	ImGui::End();
	if (BrushWindowSize.x < 200) {
		leftSize = 200;
		std::cout << leftSize << std::endl;
	}
}

void DrawUI::ServerWindow()
{
	rightSize = (((rightSize) > (rightMinSize)) ? (rightSize) : (rightMinSize));

	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, LayerWindowPos.y));

	ImGui::Begin("Lobby", nullptr, ImGuiWindowFlags_NoTitleBar | ((ServerWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	ImVec2 windowSize = ImGui::GetWindowSize();



	if (isOnline) {
		
	}
	else {
		Lss::SetFontSize(2 * Lss::VH);
		std::string text = "Wumpus is very sad :c";
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPos(ImVec2(windowSize.x / 2 - textWidth / 2, windowSize.y / 2 - 2 * Lss::VH));
		Lss::Text(text, 2 * Lss::VH);
		Lss::End();
	}

	ServerWindowSize = ImGui::GetWindowSize();
	rightSize = ServerWindowSize.x;
	ServerWindowPos = ImGui::GetWindowPos();
	ImGui::End();

	if (ServerWindowSize.x < 200) {
		rightSize = 200;
		std::cout << leftSize << std::endl;
	}
}

ImVec2 DrawLayerTreeThree(Node& node, ImVec2& cursorPos) {
	ImGui::SetCursorPos(cursorPos);
	float x = ImGui::GetContentRegionAvail().x;
	if (Folder* folder = dynamic_cast<Folder*>(&node)) {
		if (folder->selected && selectedLayer != folder->id) folder->selected = false;


		if (folder->selected) Lss::SetColor(ContainerBackground, HeavyHighlight);
		if (folder->id == 0) {
			for (int childId : folder->childrenIds) {
				Node* childNode = renderer->nodes[childId].get();
				cursorPos = DrawLayerTreeThree(*childNode, cursorPos);
			}
		}
		else {
			Lss::Child(std::to_string(folder->id) + "visibility", ImVec2(3 * Lss::VH, 3 * Lss::VH));
				Lss::LeftTop(0.25f * Lss::VW, 0.5f * Lss::VH);
				if (folder->visible) Lss::Image(visible.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
				else Lss::Image(notVisible.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
			Lss::End();
			ImGui::EndChild();

			if (ImGui::IsItemClicked()) {
				folder->visible = !folder->visible;
				std::cout << "heo" << std::endl;
			}

			ImGui::SameLine();
			Lss::Left(-0.2 * Lss::VW);

			Lss::Child(std::to_string(folder->id), ImVec2(x - 3 * Lss::VH, 3 * Lss::VH));
				Lss::SetFontSize(2 * Lss::VH);
				float validWidth = ImGui::GetContentRegionAvail().x;
				std::string percent = std::to_string(folder->opacity) + "%%";
				float textSize = ImGui::CalcTextSize((percent).c_str()).x;
				Lss::LeftTop(Lss::VH + 0.3f * Lss::VW, 0.5f * Lss::VH);
				Lss::Image(folderLayer.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
				if (!folder->editing) {
					ImGui::SameLine();
					Lss::Text(folder->name, 2 * Lss::VH);
				}
				else {
					ImGui::SameLine();
					ImGui::SetNextItemWidth(100);
					static char editBuffer[256] = "";
					bool editing = ImGui::InputText("##editinput", editBuffer, IM_ARRAYSIZE(editBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::SetKeyboardFocusHere(-1);
					if (editing) {
						if (strlen(editBuffer) > 0 && folder->name != editBuffer) {
							folder->name = editBuffer;
							memset(editBuffer, 0, sizeof(editBuffer));
							folder->editing = false;
							renderer->SendLayerRename(folder->name, folder->id);
						}
					}
				}
				ImGui::SetCursorPos(ImVec2(validWidth - textSize - Lss::VW, 0.5f * Lss::VH));
				Lss::Text(percent, 2 * Lss::VH);

			Lss::End();
			ImGui::EndChild();

			if (ImGui::IsItemClicked()) {
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					folder->editing = true;
				}
				else {
					folder->open = !folder->open;
				}

				folder->selected = true;
				selectedLayer = folder->id;
			}
			if (folder->selected) Lss::SetColor(ContainerBackground, ContainerBackground);

			if (folder->open) {
				cursorPos = ImGui::GetCursorPos();
				cursorPos.x += 2 * Lss::VW;
				for (int childId : folder->childrenIds) {
					Node* childNode = renderer->nodes[childId].get();
					DrawLayerTreeThree(*childNode, cursorPos);
				}
			}
		}
	}
	else if (Layer* layer = dynamic_cast<Layer*>(&node)) {
		if (layer->selected && selectedLayer != layer->id) layer->selected = false;


		if (layer->selected) Lss::SetColor(ContainerBackground, HeavyHighlight);
		Lss::Child(std::to_string(layer->id) + "visibility", ImVec2(3 * Lss::VH, 3 * Lss::VH));
			Lss::LeftTop(0.25f * Lss::VW, 0.5f * Lss::VH);
			if(layer->visible) Lss::Image(visible.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
			else Lss::Image(notVisible.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
		Lss::End();
		ImGui::EndChild();
		if (ImGui::IsItemClicked()) {
			layer->visible = !layer->visible;
			std::cout << "heo" << std::endl;
		}

		ImGui::SameLine();
		Lss::Left(-0.2 * Lss::VW);

		Lss::Child(std::to_string(layer->id), ImVec2(x - 3 * Lss::VH, 3 * Lss::VH));
			Lss::SetFontSize(2 * Lss::VH);
			float validWidth = ImGui::GetContentRegionAvail().x;
			std::string percent = std::to_string(layer->opacity) + "%%";
			float textSize = ImGui::CalcTextSize((percent).c_str()).x;
			Lss::LeftTop(Lss::VH + 0.3f * Lss::VW, 0.5f * Lss::VH);
			Lss::Image(layerLayer.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
			ImGui::SameLine();
			Lss::Text(layer->name, 2 * Lss::VH);
			ImGui::SetCursorPos(ImVec2(validWidth-textSize-Lss::VW, 0.5f*Lss::VH));
			Lss::Text(percent, 2 * Lss::VH);

		Lss::End();
		ImGui::EndChild();

		if(ImGui::IsItemClicked()) {
			layer->selected = true;
			selectedLayer = layer->id;
		}
		if (layer->selected) Lss::SetColor(ContainerBackground, ContainerBackground);
	}
	return ImGui::GetCursorPos();
}

void DrawLayerTreeTwo(Node& node) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (Folder* folder = dynamic_cast<Folder*>(&node)) {
		if (folder->id == 0) {
			for (int childId : folder->childrenIds) {
				Node* childNode = renderer->nodes[childId].get();
				DrawLayerTreeTwo(*childNode);
			}
		}
		else {
			bool open = ImGui::TreeNodeEx(("##" +folder->name).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (folder->open ? ImGuiTreeNodeFlags_DefaultOpen : 0));
			ImGui::SameLine();
			ImGui::Checkbox(("##" + folder->name + "visibility").c_str(), &folder->visible);
			ImGui::SameLine();
			if (folder->visible && !folder->editing) ImGui::Text((folder->name).c_str());

			if (folder->visible && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				folder->editing = true;
			}

			if (open) {
				folder->open = true;
				for (int childId : folder->childrenIds) {
					Node* childNode = renderer->nodes[childId].get();
					//childNode->visible = folder->visible;
					DrawLayerTreeTwo(*childNode);
				}
				ImGui::TreePop();
			}
			else {
				folder->open = false;
			}
			if (folder->visible && folder->editing) {
				ImGui::SameLine();
				ImGui::SetNextItemWidth(100);
				static char editBuffer[256] = "";
				bool editing = ImGui::InputText("##ChatInput", editBuffer, IM_ARRAYSIZE(editBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
				ImGui::SetKeyboardFocusHere(-1);
				if (editing) {
					if (strlen(editBuffer) > 0 && folder->name != editBuffer) {
						folder->name = editBuffer;
						memset(editBuffer, 0, sizeof(editBuffer));
						folder->editing = false;
						renderer->SendLayerRename(folder->name, folder->id);
					}
				}
			}
		}
	}
	else if (Layer* layer = dynamic_cast<Layer*>(&node)) {
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::PushID(layer->id);
		ImVec2 selectableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing());
		ImGui::Button("##SelectableLayer", selectableSize);

		bool clicked = ImGui::IsItemClicked();
		bool doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);

		if (clicked) {
			renderer->currentNode = layer->id;
		}
		ImGui::SetCursorPos(cursorPos);
		ImGui::TreeNodeEx(("##" + layer->name).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
		ImGui::SameLine();
		ImGui::Checkbox(("##" + layer->name + "visibility").c_str(), &layer->visible);
		ImGui::SameLine();
		if (layer->visible && !layer->editing) ImGui::Text((layer->name).c_str());

		if (layer->visible && doubleClicked) {
			layer->editing = true;
		}
		if (layer->visible && layer->editing) {
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			static char editBuffer[256] = "";
			bool editing = ImGui::InputText("##ChatInput", editBuffer, IM_ARRAYSIZE(editBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SetKeyboardFocusHere(-1);
			if (editing) {
				if (strlen(editBuffer) > 0 && layer->name != editBuffer) {
					layer->name = editBuffer;
					memset(editBuffer, 0, sizeof(editBuffer));
					layer->editing = false;
					renderer->SendLayerRename(layer->name, layer->id);
				}
			}
		}
		ImGui::PopID();
	}

}

void DrawUI::LayerWindow()
{
	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, ServerWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, ChatWindowPos.y - LayerWindowPos.y));

	ImGui::Begin("Layer", nullptr, ImGuiWindowFlags_NoTitleBar | ((LayerWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	Lss::SetFontSize(Lss::VH);
	Lss::Button("+", ImVec2(Lss::VW, 1.8f*Lss::VH), Lss::VH);
	Lss::Button("+2", ImVec2(Lss::VW, 1.8f * Lss::VH), Lss::VH, SameLine);
	Lss::Top(Lss::VH);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
	ImVec2 cursy = ImGui::GetCursorPos();
	DrawLayerTreeThree(*renderer->nodes[0].get(), cursy);
	ImGui::PopStyleVar();

	LayerWindowSize = ImGui::GetWindowSize();
	rightSize = LayerWindowSize.x;
	LayerWindowPos = ImGui::GetWindowPos();

	Lss::End();
	ImGui::End();
	if (LayerWindowSize.x < 200) {
		rightSize = 200;
	}
}

void DrawUI::ChatWindow()
{
	static char inputBuffer[256] = "";

	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, LayerWindowPos.y + LayerWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, windowSizeY - (LayerWindowPos.y + LayerWindowSize.y)));

	ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoTitleBar | ((ChatWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));

	float availableHeight = ImGui::GetContentRegionAvail().y;
	float inputBarHeight = ImGui::GetTextLineHeight() * 2 - ImGui::GetStyle().FramePadding.y * 2; //+ ImGui::GetStyle().FramePadding.y * 3; //+ ImGui::GetStyle().ItemSpacing.y;


	ImGui::BeginChild("ChatLog", ImVec2(0, availableHeight - inputBarHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& message : chatLog)
	{
		ImGui::TextWrapped("%s", message.c_str());
	}
	ImGui::EndChild();
	ImGui::Separator();
	ImGui::BeginChild("ChatInput", ImVec2(0, inputBarHeight - ImGui::GetStyle().ItemSpacing.y * 2), false);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::InputText("##ChatInput", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (strlen(inputBuffer) > 0)
		{
			std::string msg = runtime.username+": " + inputBuffer;
			chatLog.push_back(msg);
			SManager::SendMsg(inputBuffer);
			memset(inputBuffer, 0, sizeof(inputBuffer)); 
		}
		ImGui::SetKeyboardFocusHere(-1);
	}
	ImGui::EndChild();

	ChatWindowSize = ImGui::GetWindowSize();
	rightSize = ChatWindowSize.x;
	ChatWindowPos = ImGui::GetWindowPos();

	ImGui::End();

	if (ChatWindowSize.x < 200) {
		rightSize = 200;
	}
}

void DrawUI::InitWindow()
{
	if (!inited && !isOnline) {
		if (Lss::Modal("Canvas init", &canvasInitWindow, ImVec2(20 * Lss::VW, 40 * Lss::VH), Centered | Trans, ImGuiWindowFlags_NoDecoration))
		{
			Lss::Text("Create a Sync", 2 * Lss::VH);
			Lss::Separator();

			static char nameText[128] = "";

			Lss::Text("Name: ", 3 * Lss::VH);
			ImGui::SameLine();
			Lss::Left(4.4f * Lss::VW);
			Lss::InputText("projectName", nameText, sizeof(nameText), ImVec2(12 * Lss::VW, 3 * Lss::VH));


			ImGui::NewLine();

			static int width = 0;
			static int height = 0;

			Lss::Text("Canvas", 2 * Lss::VH);
			Lss::Separator();
			Lss::Text("Width (px): ", 3 * Lss::VH);
			ImGui::SameLine();
			Lss::Left(8.2f * Lss::VW);
			Lss::InputInt("##canvasWidth", &width, ImVec2(6 * Lss::VW, 3 * Lss::VH));
			Lss::Text("Height (px): ", 3 * Lss::VH);
			ImGui::SameLine();
			Lss::Left(7.85f * Lss::VW);
			Lss::InputInt("##canvasHeight", &height, ImVec2(6 * Lss::VW, 3 * Lss::VH));

			static char locationText[256] = "";
			static bool openExplorer = false;

			Lss::Text("Save Location", 2 * Lss::VH);
			Lss::Separator();
			Lss::SetFontSize(3 * Lss::VH);
			float buttonTextSize = ImGui::CalcTextSize("...").x;
			Lss::InputText("saveLocation", locationText, sizeof(locationText), ImVec2(20*Lss::VW-buttonTextSize-0.4f*Lss::VH, 3 * Lss::VH));
			ImGui::SameLine();
			if (Lss::Button("...", ImVec2(buttonTextSize + Lss::VH, 3.3f * Lss::VH), 3 * Lss::VH)) openExplorer = true;

			


			ImVec2 buttonSize = ImVec2(100, 20);
			ImGui::SetCursorPosY(38 * Lss::VH - buttonSize.y - 2 * Lss::VH);
			if (Lss::Button("Create##createCanvas", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered))
			{
				if (width > 0 && height > 0) {
					unsigned int widthOut = width;
					unsigned int heightOut = height;

					renderer->SetDrawData(widthOut, heightOut);

					canvasInitWindow = false;
					inited = true;
				}
			}

			Lss::End();
			ImGui::EndPopup();
		}
	}
}

void DrawUI::GetMsg(std::map<std::string, std::string> messageObject)
{
	std::string msg = messageObject["username"] + ": " + messageObject["message"];
	std::cout << msg << std::endl;
	chatLog.push_back(msg);
}