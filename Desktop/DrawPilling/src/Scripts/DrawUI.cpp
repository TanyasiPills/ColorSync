#include "DrawUI.h"
#include "HighsManager.h"
#include "SocksManager.h"
#include "RuntimeData.h"
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
int rightSize = 200;
int rightMinSize = 200;


int windowSizeX, windowSizeY;

bool inited = false;
bool needLogin = true;
bool isOnline = false;

static std::vector<std::string> chatLog;

static NewRenderer* renderer;

static RuntimeData& runtime = RuntimeData::getInstance();

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
	cursor.shader->SetUniform3f("Kolor", color[0], color[1], color[2]);
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
		bool clicked = ImGui::Selectable(("##SelectableLayer" + layer->name).c_str(), (renderer->currentNode == layer->id));
		if (clicked) {
			renderer->currentNode = layer->id;
		}
		ImGui::TreeNodeEx(("##" + layer->name).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
		ImGui::SameLine();
		ImGui::Checkbox(("##" + layer->name + "visibility").c_str(), &layer->visible);
		ImGui::SameLine();
		if (layer->visible && !layer->editing) ImGui::Text((layer->name).c_str());

		if (layer->visible && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
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
	}

}

void DrawUI::LayerWindow()
{
	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, ServerWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, ChatWindowPos.y - LayerWindowPos.y));

	ImGui::Begin("Layer", nullptr, ImGuiWindowFlags_NoTitleBar | ((LayerWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));

	DrawLayerTreeTwo(*renderer->nodes[0].get());

	LayerWindowSize = ImGui::GetWindowSize();
	rightSize = LayerWindowSize.x;
	LayerWindowPos = ImGui::GetWindowPos();
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

void DrawUI::LoginWindow()
{
	if (needLogin) {
		if (!inited) {
			ImGui::SetNextWindowPos(ImVec2(windowSizeX / 2 - 50, windowSizeY / 2 - 50));
			ImGui::SetNextWindowSize(ImVec2(200, 150)); // Adjusted window size to fit content
			inited = true;
		}
		ImGui::Begin("Nem");

		// Center the cursor in the window
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 centerPos = ImVec2(windowSize.x / 2, windowSize.y / 2);

		// Set the size for input field and position
		ImGui::SetNextItemWidth(150); // Set input width
		ImVec2 inputPos = ImVec2(centerPos.x - 75, centerPos.y - 30); // Center input horizontally
		ImGui::SetCursorPos(inputPos);

		static char usernameText[100] = "";
		ImGui::InputTextWithHint("##usernameInput", "Email", usernameText, IM_ARRAYSIZE(usernameText), ImGuiInputTextFlags_CharsNoBlank);
		inputPos.y += 30;

		ImGui::SetCursorPos(inputPos);
		ImGui::SetNextItemWidth(150);
		static char passwordText[24] = "";
		ImGui::InputTextWithHint("##passwordInput", "Password", passwordText, IM_ARRAYSIZE(passwordText), ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);

		inputPos.y += 30;
		ImGui::SetNextItemWidth(100);
		ImVec2 buttonPos = ImVec2(centerPos.x - 25, inputPos.y);
		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Login")) {
			std::thread([]() {
				nlohmann::json body;
				body["email"] = usernameText;
				body["password"] = passwordText;

				std::cout << "Sending JSON: " << body.dump() << std::endl;
				nlohmann::json res = HManager::Request((runtime.ip+":3000/users/login").c_str(), body.dump(), POST);

				if (res.contains("access_token") && res.contains("username")) {
					std::cout << "got this JSON: " << res["access_token"] << std::endl;
					runtime.token = res["access_token"];
					runtime.username = res["username"];
					needLogin = false;
				}
			}).detach();
		}

		ImGui::End();
	}
}

void DrawUI::GetMsg(std::map<std::string, std::string> messageObject)
{
	std::string msg = messageObject["username"] + ": " + messageObject["message"];
	std::cout << msg << std::endl;
	chatLog.push_back(msg);
}