#include "DrawUI.h"
#include "HighsManager.h"
#include "SocksManager.h"
#include "RuntimeData.h"
#include "FileExplorer.h"
#include "CallBacks.h"
#include "DataManager.h"
#include "lss.h"
#include <thread>

//Left side
ImVec2 ColorWindowSize(100, 300);
ImVec2 ColorWindowPos;
ImVec2 SizeWindowSize(100, 300);
ImVec2 SizeWindowPos(0, 300);
ImVec2 BrushWindowSize(100, 200);
ImVec2 BrushWindowPos(0, 600);
int leftSize = 250;
int leftMinSize = 250;

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

bool itemHovered = false;

bool usersGot = false;
std::vector<RoomUser>* usersPtr;

static std::vector<std::string> chatLog;

static NewRenderer* renderer;

static RuntimeData& runtime = RuntimeData::getInstance();

MyTexture visible;
MyTexture notVisible;
MyTexture folderLayer;
MyTexture layerLayer;

MyTexture cursor;
MyTexture pen;
MyTexture water;
MyTexture air;
MyTexture chalk;

MyTexture sizecursor;

MyTexture adminCrown;

MyTexture icons[5];

std::string names[5] = { "Cursor", "Pen Brush", "Air Brush", "Water Brush", "Chalk Brush"};

static int selected = -1;
static int selectedSize = -1;
static const ImVec2 iconSize(50, 50);

std::string savePath = "";

float startPosY = 0.0f;

void DrawUI::SetRenderer(NewRenderer& rendererIn) {
	renderer = &rendererIn;
	std::cout << "Is online: " << isOnline << std::endl;
}

void InitBrushIcons()
{
	
	cursor.Init("Resources/icons/cursorBrush.png");
	icons[0] = cursor;

	
	pen.Init("Resources/icons/penBrush.png");
	icons[1] = pen;
	
	air.Init("Resources/icons/airBrush.png");
	icons[2] = air;

	water.Init("Resources/icons/waterBrush.png");
	icons[3] = water;

	
	chalk.Init("Resources/icons/chalkBrush.png");
	icons[4] = chalk;

	sizecursor.Init("Resources/Textures/penBrush.png");
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

	adminCrown.Init("Resources/icons/crown.png");

	isOnline = renderer->GetOnline();

	InitBrushIcons();
}

static float color[3] = { 0.0f, 0.0f, 0.0f };

void DrawUI::SetColor(float* colorIn)
{
	color[0] = colorIn[0];
	color[1] = colorIn[1];
	color[2] = colorIn[2];
}

void DrawUI::DrawMenu() {
	Lss::SetFontSize(2 * Lss::VH);
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New")) {
				// Handle "New" action
			}
			if (ImGui::MenuItem("Open...")) {
				// Handle "Open" action
			}
			if (ImGui::MenuItem("Save")) {
				// Handle "Save" action
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				renderer->SwapView(isOnline);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
				// Handle Undo
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
				// Handle Redo
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "Ctrl+X")) {
				// Handle Cut
			}
			if (ImGui::MenuItem("Copy", "Ctrl+C")) {
				// Handle Copy
			}
			if (ImGui::MenuItem("Paste", "Ctrl+V")) {
				// Handle Paste
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			bool showGrid = true;
			ImGui::MenuItem("Show Grid", nullptr, &showGrid);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	startPosY = ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y + ImGui::GetStyle().FramePadding.y;
}

void DrawUI::ColorWindow(RenderData& cursor)
{
	ImGui::SetNextWindowPos(ImVec2(0, startPosY), ImGuiCond_Always);
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

		int sliderVal = cursorRadius * 100;
		sliderVal -= 1;
		sliderVal *= 2;
		if (sliderVal == 0) sliderVal = 1;
		ImGui::SliderInt("##Scale", &sliderVal, 1, 16);
		if (sliderVal == 1) sliderVal = 0;
		sliderVal /= 2;
		sliderVal += 1;
		
		cursorRadius = float(sliderVal) / 100.0f;

		ImGui::Columns(3, nullptr, false);

		for (int i = 1; i < 10; i++)
		{
			int index = i - 1;
			if ((index * 0.01f + 0.01f) <= cursorRadius && cursorRadius < ((index + 1) * 0.01f + 0.01f)) selectedSize = index;

			bool isSelected = (index == selectedSize);
			if (ImGui::Selectable(("##" + std::to_string(index) + "sizes").c_str(), isSelected, 0, ImVec2(0, iconSize.y + 2 * Lss::VH))) {
				cursorRadius = index * 0.01f + 0.01f;
				selectedSize = index;
			}

			ImVec2 pos = ImGui::GetItemRectMin();
			ImVec2 size = ImGui::GetItemRectSize();
			ImVec2 cursorPos = ImVec2(pos.x + size.x / 2 - iconSize.x / 2, pos.y);
			ImGui::SetCursorScreenPos(cursorPos);

			float zoom = 0.025f * index;
			Lss::Image(sizecursor.GetId(), iconSize, 0, ImVec2(zoom, zoom), ImVec2(1 - zoom, 1 - zoom));
			Lss::SetFontSize(2 * Lss::VH);

			std::string nameStuff = (index == 0) ? std::to_string(i) : std::to_string(index*2);
			float nameSize = ImGui::CalcTextSize(nameStuff.c_str()).x;
			ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.x / 2) - (nameSize / 2), ImGui::GetCursorScreenPos().y));
			Lss::Text(nameStuff, 2 * Lss::VH);

			ImGui::NextColumn();
			Lss::End();
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

	ImGui::Columns(2, nullptr, false);
	int index = 0;


	for (RenderData brush : renderer->brushes)
	{
		if (index <= 4) {
			bool isSelected = (index == selected);
			if (ImGui::Selectable(("##" + std::to_string(index)).c_str(), isSelected, 0, ImVec2(0, iconSize.y + 2 * Lss::VH))) {
				if (!isSelected) renderer->ChangeBrush(index);
				selected = index;
			}
			ImVec2 pos = ImGui::GetItemRectMin();
			ImVec2 size = ImGui::GetItemRectSize();
			ImVec2 cursorPos = ImVec2(pos.x + size.x / 2 - iconSize.x / 2, pos.y);
			ImGui::SetCursorScreenPos(cursorPos);

			Lss::Image(icons[index].GetId(), iconSize);
			Lss::SetFontSize(2 * Lss::VH);

			std::string fileNameStuff = names[index];

			float nameSize = ImGui::CalcTextSize(fileNameStuff.c_str()).x;
			ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.x / 2) - (nameSize / 2), ImGui::GetCursorScreenPos().y));
			Lss::Text(fileNameStuff, 2 * Lss::VH);
		}
		index++;
		ImGui::NextColumn();
		Lss::End();
	}
	ImGui::End();
}

void DrawUI::ServerWindow()
{
	rightSize = (((rightSize) > (rightMinSize)) ? (rightSize) : (rightMinSize));

	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, startPosY), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, LayerWindowPos.y));

	ImGui::Begin("Lobby", nullptr, ImGuiWindowFlags_NoTitleBar | ((ServerWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	ImVec2 windowSize = ImGui::GetWindowSize();
	Lss::SetFontSize(2 * Lss::VH);
	if (Lss::Button("Save", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH))
	{
		DataManager::SaveSyncData(savePath);
	}
	if (Lss::Button("Load", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH, SameLine))
	{
		DataManager::LoadSyncData(savePath);
	}

	if (isOnline) {
		if (!usersGot) {
			usersPtr = SManager::GetUsers();
			usersGot = true;
		}
		ImVec2 avail = ImGui::GetContentRegionAvail();
		for (RoomUser user : *usersPtr)
		{
			Lss::Child("userOnServer" + std::to_string(user.id), ImVec2(avail.x, 4 * Lss::VH),false, Rounded);
				if (user.id == runtime.id) {
					Lss::Text(user.username+" - ME", 4 * Lss::VH);
				}
				else Lss::Text(user.username, 4 * Lss::VH);
				if (user.admin) Lss::Image(adminCrown.GetId(), ImVec2(4*Lss::VH, 4*Lss::VH),SameLine);
				if (SManager::AmIOwner()) {
					if (Lss::Button("Kick", ImVec2(4 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH))
						SManager::Kick(user.id);
				}
				Lss::End();
			ImGui::EndChild();
		}
		Lss::End();
	}
	else {
		std::string text = "Wumpus is very sad :c";
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPos(ImVec2(windowSize.x / 2 - textWidth / 2, windowSize.y / 2 - 2 * Lss::VH));
		Lss::Text(text, 2 * Lss::VH);
		Lss::End();
	}

	ServerWindowSize = ImGui::GetWindowSize();
	rightSize = ServerWindowSize.x;
	ServerWindowPos = ImGui::GetWindowPos();
	Lss::End();
	ImGui::End();

	if (ServerWindowSize.x < 200) {
		rightSize = 200;
		std::cout << leftSize << std::endl;
	}
}

void ChangeVisibilityChild(int& index)
{
	Folder* foldy = dynamic_cast<Folder*>(renderer->nodes[index].get());

	std::vector<int> childrenCopy = foldy->childrenIds;

	for (int child : childrenCopy)
	{
		if (renderer->nodes.find(child) == renderer->nodes.end()) continue;

		if (Folder* childFoldy = dynamic_cast<Folder*>(renderer->nodes[child].get()))
		{
			ChangeVisibilityChild(child);
		}
		renderer->nodes[child]->visible = foldy->visible;
	}
}

ImVec2 DrawLayerTreeThree(Node& node, ImVec2& cursorPos) {
	ImGui::SetCursorPos(cursorPos);
	float x = ImGui::GetContentRegionAvail().x;
	float cursorX = ImGui::GetCursorPosX();
	Node* nody = &node;
	Folder* foldy = nullptr;

	bool isFolder = dynamic_cast<Folder*>(&node) != nullptr;
	if (isFolder) foldy = dynamic_cast<Folder*>(&node);
	if (!nody) return cursorPos;
	if (nody->selected && selectedLayer != nody->id) nody->selected = false;
	if (nody->selected) Lss::SetColor(ContainerBackground, HeavyHighlight);
	if (isFolder) {
		if (foldy->id == 0) {
			for (int childId : foldy->childrenIds) {
				Node* childNode = renderer->nodes[childId].get();
				cursorPos = DrawLayerTreeThree(*childNode, cursorPos);
			}
			return cursorPos;
		}
	}

	Lss::Child(std::to_string(nody->id) + "visibility", ImVec2(3 * Lss::VH, 3 * Lss::VH));
	Lss::LeftTop(0.25f * Lss::VW, 0.5f * Lss::VH);
	if (nody->visible) Lss::Image(visible.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
	else Lss::Image(notVisible.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
	Lss::End();
	ImGui::EndChild();

	if (ImGui::IsItemClicked()) {
		nody->visible = !nody->visible;
		if(isFolder) ChangeVisibilityChild(nody->id);
		itemHovered = true;
	}

	ImGui::SameLine();
	Lss::Left(-0.2 * Lss::VW);

	Lss::Child(std::to_string(nody->id), ImVec2(x - 3 * Lss::VH, 3 * Lss::VH));
	Lss::SetFontSize(2 * Lss::VH);
	float validWidth = ImGui::GetContentRegionAvail().x;
	std::string percent = std::to_string(nody->opacity) + "%%";
	float textSize = ImGui::CalcTextSize((percent).c_str()).x;
	Lss::LeftTop(Lss::VH + 0.3f * Lss::VW, 0.5f * Lss::VH);
	if (isFolder) Lss::Image(folderLayer.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
	else Lss::Image(layerLayer.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH));
	if (!nody->editing) {
		ImGui::SameLine();
		Lss::Text(nody->name, 2 * Lss::VH);
	}
	else {
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		static char editBuffer[256] = "";
		bool editing = ImGui::InputText("##editinput", editBuffer, IM_ARRAYSIZE(editBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SetKeyboardFocusHere(-1);
		if (editing) {
			if (strlen(editBuffer) > 0 && nody->name != editBuffer) {
				nody->name = editBuffer;
				memset(editBuffer, 0, sizeof(editBuffer));
				nody->editing = false;
				if (renderer->GetOnline() == true){
					NodeRenameMessage renameMessage;
					renameMessage.name = nody->name;
					renameMessage.location = nody->id;
					renameMessage.type = RenameNode;
					SManager::SendAction(renameMessage);
				}
			}
		}
	}
	ImGui::SetCursorPos(ImVec2(validWidth - textSize - Lss::VW, 0.5f * Lss::VH));
	Lss::Text(percent, 2 * Lss::VH);

	Lss::End();
	ImGui::EndChild();
	if (ImGui::IsItemClicked()) {
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			std::cout << "pressed on: " << nody->name << std::endl;
			nody->editing = true;
		}
		else {
			if (isFolder) foldy->open = !foldy->open;
			else renderer->currentNode = nody->id;
		}
	
		nody->selected = true;
		selectedLayer = nody->id;
		itemHovered = true;
	}

	if (nody->selected) Lss::SetColor(ContainerBackground, ContainerBackground);

	if (isFolder && foldy->open) {
		cursorPos = ImGui::GetCursorPos();
		for (int childId : foldy->childrenIds) {
			cursorPos.x = cursorX;
			cursorPos.x += 2 * Lss::VW;
			Node* childNode = renderer->nodes[childId].get();
			cursorPos = DrawLayerTreeThree(*childNode, cursorPos);
		}
		cursorPos.x += 2 * Lss::VW;
	}
	return ImGui::GetCursorPos();
}

void DrawUI::DeleteChilds(int& index)
{
	Folder* foldy = dynamic_cast<Folder*>(renderer->nodes[index].get());

	std::vector<int> childrenCopy = foldy->childrenIds;

	for (int child : childrenCopy)
	{
		if (renderer->nodes.find(child) == renderer->nodes.end()) continue;

		if (Folder* childFoldy = dynamic_cast<Folder*>(renderer->nodes[child].get()))
		{
			DeleteChilds(child);
			renderer->RemoveFolder(child);
		}
		else {
			renderer->RemoveLayer(child);
		}
	}
	foldy->childrenIds.clear();
}

void DrawUI::LayerWindow()
{
	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, ServerWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, ChatWindowPos.y - LayerWindowPos.y));

	itemHovered = false;

	ImGui::Begin("Layer", nullptr, ImGuiWindowFlags_NoTitleBar | ((LayerWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	itemHovered = !ImGui::IsWindowHovered();
	Lss::SetFontSize(Lss::VH);
	if (Lss::Button("+", ImVec2(Lss::VW, 1.8f * Lss::VH), Lss::VH)) {
		int parentToSend = 0;
		if (selectedLayer == -1) {
			int parent = 0;
			renderer->CreateLayer(parent);
		}
		else {
			if (dynamic_cast<Folder*>(renderer->nodes[selectedLayer].get())) {
				parentToSend = selectedLayer;
				renderer->CreateLayer(selectedLayer);
			}
			else {
				int parent = renderer->GetParent(selectedLayer);
				parentToSend = parent;
				renderer->CreateLayer(parent);
			}
		}
		if (renderer->GetOnline() == true) {
			NodeAddMessage layerMessage;
			layerMessage.location = parentToSend;
			layerMessage.nodeType = 0;
			layerMessage.type = AddNode;
			SManager::SendAction(layerMessage);
		}
		renderer->nextFreeNodeIndex++;
	}
	if(ImGui::IsItemHovered()) itemHovered = true;
	if (Lss::Button("+2", ImVec2(Lss::VW, 1.8f * Lss::VH), Lss::VH, SameLine))
	{
		int parentToSend = 0;
		if (selectedLayer == -1) {
			int parent = 0;
			renderer->CreateFolder(parent);
		}
		else {
			if (dynamic_cast<Folder*>(renderer->nodes[selectedLayer].get())) {
				parentToSend = selectedLayer;
				renderer->CreateFolder(selectedLayer);
			}
			else {
				int parent = renderer->GetParent(selectedLayer);
				parentToSend = selectedLayer;
				renderer->CreateFolder(parent);
			}
		}
		if (renderer->GetOnline() == true) {
			NodeAddMessage layerMessage;
			layerMessage.location = parentToSend;
			layerMessage.nodeType = 1;
			layerMessage.type = AddNode;
			SManager::SendAction(layerMessage);
		}
		renderer->nextFreeNodeIndex++;
	}
	if (ImGui::IsItemHovered()) itemHovered = true;
	if (Lss::Button("-", ImVec2(Lss::VW, 1.8f * Lss::VH), Lss::VH, SameLine))
	{
		if (selectedLayer != -1) {
			if (dynamic_cast<Folder*>(renderer->nodes[selectedLayer].get())) {
				DeleteChilds(selectedLayer);
				renderer->RemoveFolder(selectedLayer);

			}
			else {
				renderer->RemoveLayer(selectedLayer);
			}
			if (renderer->GetOnline() == true) {
				NodeDeleteMessage deleteMessage;
				deleteMessage.location = selectedLayer;
				deleteMessage.type = DeleteNode;
				SManager::SendAction(deleteMessage);
			}
		}
	}
	if (ImGui::IsItemHovered()) itemHovered = true;
	Lss::Top(Lss::VH);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
	ImVec2 cursy = ImGui::GetCursorPos();
	if(renderer->inited) DrawLayerTreeThree(*renderer->nodes[0].get(), cursy);
	ImGui::PopStyleVar();

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !itemHovered)
	{
		selectedLayer = -1;
	}

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
	ImVec2 avail = ImGui::GetContentRegionAvail();

	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, LayerWindowPos.y + LayerWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, windowSizeY - (LayerWindowPos.y + LayerWindowSize.y)));
	float heightOfChat = windowSizeY - (LayerWindowPos.y + LayerWindowSize.y);
	ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoTitleBar | ((ChatWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));

	ImGui::BeginChild("ChatLog", ImVec2(0, heightOfChat-4.8f*Lss::VH), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& message : chatLog)
	{
		Lss::SetFontSize(2 * Lss::VH);
		ImGui::TextWrapped("%s", message.c_str());
	}
	Lss::End();
	ImGui::EndChild();
	ImGui::Separator();
	ImGui::BeginChild("ChatInput", ImVec2(0, 2.5f * Lss::VH), false);

	static char inputBuffer[256] = "";

	if (Lss::InputText("chatInput", inputBuffer, sizeof(inputBuffer), ImVec2(avail.x, 2*Lss::VH), 0, ImGuiInputTextFlags_EnterReturnsTrue))
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
	Lss::End();
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

			static int width = 400;
			static int height = 300;

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
			static bool wasOpen = false;

			Lss::Text("Save Location", 2 * Lss::VH);
			Lss::Separator();
			Lss::SetFontSize(3 * Lss::VH);
			float buttonTextSize = ImGui::CalcTextSize("...").x;
			Lss::InputText("saveLocation", locationText, sizeof(locationText), ImVec2(20*Lss::VW-buttonTextSize-0.4f*Lss::VH, 3 * Lss::VH),0, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (Lss::Button("...", ImVec2(buttonTextSize + Lss::VH, 3.3f * Lss::VH), 3 * Lss::VH)) {
				openExplorer = true;
				wasOpen = true;
			}
			if(openExplorer) Explorer::FileExplorerUI(&openExplorer, 4);
			else if (wasOpen)
			{
				strcpy(locationText, Explorer::GetImagePath().c_str());
				savePath = Explorer::GetImagePath();
			}
			
			ImVec2 buttonSize = ImVec2(100, 20);
			ImGui::SetCursorPosY(38 * Lss::VH - buttonSize.y - 2 * Lss::VH);
			if (Lss::Button("Create##createCanvas", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered))
			{
				if (Explorer::Exists())
				{
					DataManager::LoadSyncData(savePath);

					canvasInitWindow = false;
					inited = true;
					std::cout << "heoooo" << std::endl;
				}
				else {
					if (width > 0 && height > 0) {
						unsigned int widthOut = width;
						unsigned int heightOut = height;
						renderer->SetDrawData(widthOut, heightOut);
						renderer->InitNewCanvas();

						canvasInitWindow = false;
						inited = true;
					}
				}

			}

			Lss::End();
			ImGui::EndPopup();
		}
	}
	else if (!inited && isOnline)
	{
		unsigned int* sizes = SManager::GetCanvasSize();
		renderer->SetDrawData(sizes[0], sizes[1]);
		renderer->InitNewCanvas();
		inited = true;
	}
}

void DrawUI::GetMsg(std::map<std::string, std::string> messageObject)
{
	std::string msg = messageObject["username"] + ": " + messageObject["message"];
	std::cout << msg << std::endl;
	chatLog.push_back(msg);
}