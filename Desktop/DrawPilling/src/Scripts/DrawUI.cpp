#include "DrawUI.h"
#include "HighsManager.h"
#include "SocksManager.h"
#include "RuntimeData.h"
#include "FileExplorer.h"
#include "CallBacks.h"
#include "DataManager.h"
#include "lss.h"
#include <thread>
#include <random>
#include <memory>

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

std::unique_ptr<std::unordered_map<int, UserPos>> DrawUI::userPositions = std::make_unique<std::unordered_map<int, UserPos>>();

static std::vector<std::string> chatLog;

static NewRenderer* renderer;

static RuntimeData& runtime = RuntimeData::getInstance();

MyTexture visible;
MyTexture notVisible;
MyTexture folderLayer;
MyTexture layerLayer;

MyTexture cursor;
MyTexture pen;
MyTexture eraser;
MyTexture water;
MyTexture air;
MyTexture chalk;

MyTexture playerCursor;

MyTexture sizecursor;

MyTexture adminCrown;

MyTexture deleteStuff;
MyTexture addFolder;
MyTexture addLayer;

MyTexture icons[6];

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(0, 6);

std::string names[6] = { "Cursor", "Pen Brush", "Eraser", "Air Brush", "Water Brush", "Chalk Brush"};

bool DrawUI::canInit = false;

ImVec4 userColors[] = {
	ImVec4(0.2588f, 0.5294f, 0.9608f, 1.0f),
	ImVec4(0.1373f, 0.8353f, 0.8588f, 1.0f),
	ImVec4(0.1373f, 0.8588f, 0.2471f, 1.0f),
	ImVec4(0.9059f, 0.9804f, 0.2549f, 1.0f),
	ImVec4(0.8745f, 0.1725f, 0.9098f, 1.0f),
	ImVec4(0.8196f, 0.0157f, 0.0157f, 1.0f),
	ImVec4(1.0f, 0.6667f, 0.0f, 1.0f)
};

static int userColor;

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
	playerCursor.Init("Resources/Textures/user.png");
	playerCursor.TransparencyCorrection();
	playerCursor.BlendCorrection();

	cursor.Init("Resources/icons/cursorBrush.png");
	cursor.TransparencyCorrection();
	cursor.BlendCorrection();
	icons[0] = cursor;

	pen.Init("Resources/icons/penBrush.png");
	pen.TransparencyCorrection();
	pen.BlendCorrection();
	icons[1] = pen;

	eraser.Init("Resources/icons/eraser.png");
	eraser.TransparencyCorrection();
	eraser.BlendCorrection();
	icons[2] = eraser;
	
	air.Init("Resources/icons/airBrush.png");
	air.TransparencyCorrection();
	air.BlendCorrection();
	icons[3] = air;

	water.Init("Resources/icons/waterBrush.png");
	water.TransparencyCorrection();
	water.BlendCorrection();
	icons[4] = water;

	chalk.Init("Resources/icons/chalkBrush.png");
	chalk.TransparencyCorrection();
	chalk.BlendCorrection();
	icons[5] = chalk;

	sizecursor.Init("Resources/icons/size.png");
	sizecursor.TransparencyCorrection();
	sizecursor.BlendCorrection();
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
	visible.BlendCorrection();
	notVisible.Init("Resources/icons/eyeClosed.png");
	notVisible.BlendCorrection();
	folderLayer.Init("Resources/icons/folderLayer.png");
	folderLayer.BlendCorrection();
	layerLayer.Init("Resources/icons/layer.png");
	layerLayer.BlendCorrection();

	adminCrown.Init("Resources/icons/crown.png");
	adminCrown.BlendCorrection();

	deleteStuff.Init("Resources/icons/trash.png");
	sizecursor.TransparencyCorrection();
	adminCrown.BlendCorrection();
	addFolder.Init("Resources/icons/newFolder.png");
	sizecursor.TransparencyCorrection();
	adminCrown.BlendCorrection();
	addLayer.Init("Resources/icons/newLayer.png");
	sizecursor.TransparencyCorrection();
	adminCrown.BlendCorrection();


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
	static bool openExplorer = false;
	static bool wasOpen = false;
	static bool needFileOpen = false;
	static bool needFileSave = false;

	Lss::SetFontSize(2 * Lss::VH);
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New")) {
				canvasInitWindow = true;
				inited = false;
				selected = 0;
				renderer->tool = 0;
			}
			if (ImGui::MenuItem("Open...")) {
				if(!isOnline){
					openExplorer = true;
					wasOpen = true;
					needFileOpen = true;
				}
			}
			if (ImGui::MenuItem("Save")) {
				if (savePath.empty()) {
					openExplorer = true;
					wasOpen = true;
					needFileSave = true;
				}
				else DataManager::SaveSyncData(savePath);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				renderer->SwapView(isOnline);
			}
			ImGui::EndMenu();
		}

		if (openExplorer) Explorer::FileExplorerUI(&openExplorer, 3);
		else if (wasOpen)
		{
			savePath = Explorer::GetImagePath();
			if (Explorer::Exists()) {
				DataManager::LoadSyncData(savePath);
			}
			wasOpen = false;
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
		Lss::End();
		ImGui::EndMainMenuBar();
	}
	Lss::End();

	startPosY = ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y + ImGui::GetStyle().FramePadding.y;
}

void DrawUI::PlayerVisualization() {
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	bool open = true;
	Lss::SetFontSize(2 * Lss::VH);
	for (auto& pair : *userPositions) {
		UserPos posy = pair.second;
		ImVec2 textSize = ImGui::CalcTextSize(posy.name.c_str());
		ImVec2 size = ImVec2(textSize.x, textSize.y + 4 * Lss::VH);
		ImGui::SetNextWindowSize(size);
		posy.pos.x = (posy.pos.x + 1.0f) / 2;
		posy.pos.y = (posy.pos.y + 1.0f) / 2;
		ImVec2 realPos = ImVec2(windowSize.x * posy.pos.x, windowSize.y - (windowSize.y * posy.pos.y));
		ImGui::SetNextWindowPos(ImVec2(realPos.x - (size.x / 2), realPos.y - (size.y / 3)));
		ImGui::Begin("palyerWindow", &open, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
			Lss::Top(0.25f * Lss::VH);
			Lss::Image(playerCursor.GetId(), ImVec2(2 * Lss::VH, 2 * Lss::VH), Centered, ImVec2(0, 0), ImVec2(1, 1), userColors[posy.color]);
			ImGui::GetStyle().Colors[ImGuiCol_Text] = userColors[posy.color];
			Lss::Text(posy.name, 1.5f * Lss::VH, Centered);
			Lss::SetColor(Font, Font);
			Lss::End();
		ImGui::End();
		Lss::End();
	}
}

void DrawUI::ColorWindow(RenderData& cursor)
{
	ImGui::SetNextWindowPos(ImVec2(0, startPosY), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftSize, SizeWindowPos.y-startPosY));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Lss::VH / 6);
	ImGui::Begin("Color", nullptr, ImGuiWindowFlags_NoTitleBar |  ImGuiWindowFlags_NoResize);
	ImGui::PopStyleVar();
	ImGui::SetNextItemWidth(-1);
	ImGui::ColorEdit3("##c", color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoLabel);
	ImGui::SetNextItemWidth(-1);
	ImGui::ColorPicker3("##MyColor##6", (float*)&color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
	if(renderer->inited)cursor.shader->SetUniform4f("Kolor", color[0], color[1], color[2], 1.0f);
	renderer->SetColor(color);

	ColorWindowSize = ImGui::GetWindowSize();
	leftSize = ColorWindowSize.x;
	ColorWindowPos = ImGui::GetWindowPos();
	ImGui::End();
	if (ColorWindowSize.x < 200) {
		leftSize = 200;
		std::cout << leftSize << std::endl;
	}
	/*
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;  // Get the screen size
	ImVec2 windowPos((windowSize.x - (20 * Lss::VH)) / 2, (windowSize.y - (20 * Lss::VH)) / 2); // Calculate the center

	ImGui::SetNextWindowPos(windowPos);  // Set the window position
	ImGui::SetNextWindowSize(ImVec2(20 * Lss::VH, 20 * Lss::VH));  // Set the window size
	ImGui::Begin("test", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
		Lss::Button("Semmi sus", ImVec2(10 * Lss::VH, 3 * Lss::VH), 2 * Lss::VH, Centered | Rounded);
	ImGui::End();
	*/
}

void DrawUI::SizeWindow(float& cursorRadius, float scale)
{
	ImGui::SetNextWindowPos(ImVec2(0, ColorWindowSize.y+startPosY), ImGuiCond_Always);

	ImGui::SetNextWindowSize(ImVec2(leftSize, BrushWindowPos.y - SizeWindowPos.y));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Lss::VH / 6);
	ImGui::Begin("Size", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::PopStyleVar();

		static int lastVisual = -1;

		float sliderVal = (cursorRadius / scale) * 100;
		int visual = (int)sliderVal;
		ImGui::SliderInt("##Scale", &visual, 1, 16);
		bool sliderHeld = ImGui::IsItemActive();
		if (sliderHeld) {
			float floatPart = fmodf(cursorRadius / scale * 100.0f, 1.0f);
			sliderVal = visual + floatPart;
			cursorRadius = sliderVal * scale / 100.0f;
		}


		ImGui::Columns(3, nullptr, false);

		if (sliderHeld && visual != lastVisual) {
			for (int i = 1; i < 10; i++) {
				int index = i - 1;
				if ((index * 2 == visual) || (index == 0 && visual == 1)) selectedSize = index;
			}
			std::cout << selectedSize << std::endl;
			std::cout << "ya" << std::endl;
			lastVisual = visual;
		}

		for (int i = 1; i < 10; i++)
		{
			int index = i - 1;

			bool isSelected = (index == selectedSize);
			if (ImGui::Selectable(("##" + std::to_string(index) + "sizes").c_str(), isSelected, 0, ImVec2(0, iconSize.y + 2 * Lss::VH))) {
				float calcIndex = (index == 0) ? 1 : index * 2;
				cursorRadius = calcIndex * scale / 100.0f;
				selectedSize = index;
			}

			ImVec2 pos = ImGui::GetItemRectMin();
			ImVec2 size = ImGui::GetItemRectSize();
			ImVec2 cursorPos = ImVec2(pos.x + size.x / 2 - iconSize.x / 2, pos.y);
			ImGui::SetCursorScreenPos(cursorPos);

			float zoom = 0.04f * index;
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

void DrawUI::BrushWindow(GLFWwindow* window, RenderData& cursor) 
{
	glfwGetWindowSize(window, &windowSizeX, &windowSizeY);
	ImGui::SetNextWindowPos(ImVec2(0, SizeWindowPos.y + SizeWindowSize.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftSize, windowSizeY - (SizeWindowPos.y + SizeWindowSize.y)));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Lss::VH / 6);
	ImGui::Begin("Brushes", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::PopStyleVar();
	ImGui::Columns(2, nullptr, false);
	int index = 0;


	for (RenderData brush : renderer->brushes)
	{
		if (index <= 5) {
			bool isSelected = (index == selected);
			if (ImGui::Selectable(("##" + std::to_string(index)).c_str(), isSelected, 0, ImVec2(0, iconSize.y + 2 * Lss::VH))) {
				if (!isSelected) {
					renderer->ChangeBrush(index);
					selected = index;
					renderer->tool = index;
				}
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
	ImGui::SetNextWindowSize(ImVec2(rightSize, ServerWindowSize.y));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Lss::VH / 6);
	ImGui::Begin("Lobby", nullptr, ImGuiWindowFlags_NoTitleBar | ((ServerWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	ImGui::PopStyleVar();
	ImVec2 windowSize = ImGui::GetWindowSize();
	Lss::SetFontSize(2 * Lss::VH);
	if (isOnline) {
		if (!usersGot) {
			usersPtr = SManager::GetUsers();
			usersGot = true;
		}
		ImVec2 avail = ImGui::GetContentRegionAvail();
		int sizeofarray = DrawUI::userPositions->size();
		for (RoomUser user : *usersPtr)
		{
			Lss::Child("userOnServer" + std::to_string(user.id), ImVec2(avail.x, 4 * Lss::VH),false, Rounded, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				Lss::Left(Lss::VH);
				if (user.id == runtime.id) {
					Lss::Text(user.username, 4 * Lss::VH);
				}
				else Lss::Text(user.username, 4 * Lss::VH);
				if (user.admin) Lss::Image(adminCrown.GetId(), ImVec2(4*Lss::VH, 4*Lss::VH),SameLine);
				if (SManager::AmIOwner() && user.id != runtime.id) {
					if (Lss::Button("Kick", ImVec2(4 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH))
						SManager::Kick(user.id);
				}
				Lss::End();
			ImGui::EndChild();

			if (user.id == runtime.id) continue;

			if (userPositions->find(user.id) == userPositions->end()) {
				UserPos pos;
				pos.color = dist(gen);
				pos.name = user.username;
				pos.pos = Position(0, -100);
				auto& map = *DrawUI::userPositions;
				map[user.id] = pos;
			}
		}
		Lss::End();
	}
	
	else {
		std::string text = "The editor is in offline mode";
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
	}
}

void ChangeOpacityChild(int& index)
{
	if (Folder* foldy = dynamic_cast<Folder*>(renderer->nodes[index].get())) {
		std::vector<int> childrenCopy = foldy->childrenIds;

		for (int child : childrenCopy)
		{
			if (renderer->nodes.find(child) == renderer->nodes.end()) continue;

			if (Folder* childFoldy = dynamic_cast<Folder*>(renderer->nodes[child].get()))
			{
				ChangeOpacityChild(child);
			}
			renderer->nodes[child]->opacity = foldy->opacity;
		}
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
	ImGui::SetNextWindowPos(ImVec2(windowSizeX - rightSize, ServerWindowSize.y+startPosY), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightSize, LayerWindowSize.y));

	itemHovered = false;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Lss::VH / 6);
	ImGui::Begin("Layer", nullptr, ImGuiWindowFlags_NoTitleBar | ((LayerWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	ImGui::PopStyleVar();
	itemHovered = !ImGui::IsWindowHovered();
	Lss::SetFontSize(Lss::VH);
	if (ImGui::ImageButton("+layer", addLayer.GetId(), ImVec2(Lss::VW, Lss::VW))) {
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
	ImGui::SameLine();
	if (ImGui::ImageButton("+folder", addFolder.GetId(), ImVec2(Lss::VW, Lss::VW)))
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
	ImGui::SameLine();
	if (ImGui::ImageButton("-minden", deleteStuff.GetId(), ImVec2(Lss::VW, Lss::VW)))
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
	if (selectedLayer > -1) {
		ImGui::SameLine();
		ImGui::SliderInt("Opacity", &renderer->nodes[selectedLayer].get()->opacity, 0, 100);
		ChangeOpacityChild(selectedLayer);
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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Lss::VH / 6);
	ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoTitleBar | ((ChatWindowSize.x < 200) ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None));
	ImGui::PopStyleVar();

	ImGui::BeginChild("ChatLog", ImVec2(0, heightOfChat-4.8f*Lss::VH), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& message : chatLog)
	{
		Lss::SetFontSize(2 * Lss::VH);
		ImGui::TextWrapped("%s", message.c_str());
	}
	Lss::End();
	ImGui::EndChild();
	ImGui::Separator();

	static char inputBuffer[256] = "";

	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.0627f, 0.0627f, 0.1451f, 1.0f);
	if (Lss::InputText("chatInput", inputBuffer, sizeof(inputBuffer), ImVec2(avail.x, 2*Lss::VH), Rounded, ImGuiInputTextFlags_EnterReturnsTrue))
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
	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.1059f, 0.1059f, 0.2980f, 1.0f);

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
		if (Lss::Modal("Canvas init", &canvasInitWindow, ImVec2(20 * Lss::VW, 35 * Lss::VH), Centered | Rounded | Bordering, ImGuiWindowFlags_NoDecoration))
		{

			ImVec2 valid = ImGui::GetContentRegionAvail();

			ImVec2 def = ImGui::GetStyle().FramePadding;
			ImGui::GetStyle().FramePadding = ImVec2(0.0f, 0.0f);

			Lss::SetColor(ContainerBackground, LowHighlight);
			Lss::Child("##Lobbyheader", ImVec2(valid.x, 7 * Lss::VH), false, Rounded);
			Lss::LeftTop(Lss::VW, Lss::VH);
			Lss::Text("Create a sync", 5 * Lss::VH);
			Lss::End();
			ImGui::EndChild();
			Lss::SetColor(ContainerBackground, ContainerBackground);
			ImGui::GetStyle().FramePadding = def;

			ImGui::SetCursorPosX(0);
			Lss::Top(0.2f*Lss::VH);
			Lss::Separator(1.0f, 20 * Lss::VW);

			Lss::Top(Lss::VH);

			static int width = 400;
			static int height = 300;

			float sizeForWidth = ImGui::CalcTextSize("Width(px): ").x;
			float sizeForHeight = ImGui::CalcTextSize("Height(px): ").x;

			Lss::Text("Canvas", 2 * Lss::VH);
			Lss::Separator();

			Lss::Left(Lss::VW);
			Lss::Text("Width (px): ", 2.5f * Lss::VH);
			ImGui::SameLine();
			Lss::Left(sizeForHeight-sizeForWidth + 5.3f * Lss::VW);
			Lss::InputInt("##canvasWidth", &width, ImVec2(6 * Lss::VW, 3 * Lss::VH), Rounded);
			
			Lss::LeftTop(Lss::VW, 0.5f*Lss::VH);
			Lss::Text("Height (px): ", 2.5f * Lss::VH);
			ImGui::SameLine();
			Lss::Left(5.2f * Lss::VW);
			Lss::InputInt("##canvasHeight", &height, ImVec2(6 * Lss::VW, 3 * Lss::VH), Rounded);

			static char locationText[256] = "";
			static bool openExplorer = false;
			static bool wasOpen = false;

			Lss::Top(Lss::VH);
			Lss::Text("Save Location", 2 * Lss::VH);
			Lss::Separator();
			Lss::SetFontSize(3 * Lss::VH);
			float buttonTextSize = ImGui::CalcTextSize("...").x;
			Lss::LeftTop(Lss::VW, 0.5f * Lss::VH);
			Lss::InputText("saveLocation", locationText, sizeof(locationText), ImVec2(20*Lss::VW-buttonTextSize-3*Lss::VW, 3 * Lss::VH), Rounded, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			Lss::Left(0.8f*Lss::VW);
			if (Lss::Button("...", ImVec2(buttonTextSize + Lss::VH, 3 * Lss::VH), 2.5f * Lss::VH, Rounded)) {
				openExplorer = true;
				wasOpen = true;
			}
			if(openExplorer) Explorer::FileExplorerUI(&openExplorer, 3);
			else if (wasOpen)
			{
				strcpy(locationText, Explorer::GetImagePath().c_str());
				savePath = Explorer::GetImagePath();
			}
			
			ImVec2 buttonSize = ImVec2(100, 20);
			ImGui::SetCursorPosY(33 * Lss::VH - buttonSize.y - 2 * Lss::VH);
			if (Lss::Button("Create##createCanvas", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered | Rounded))
			{
				if (savePath.size() > 3) {
					if (Explorer::Exists())
					{
						DataManager::LoadSyncData(savePath);

						canvasInitWindow = false;
						inited = true;
						selectedLayer = 2;
					}
					else {
						if (width > 0 && height > 0) {
							unsigned int widthOut = width;
							unsigned int heightOut = height;
							renderer->SetDrawData(widthOut, heightOut);
							renderer->InitNewCanvas();

							canvasInitWindow = false;
							inited = true;
							selectedLayer = 2;
						}
					}
				}
			}

			Lss::End();
			ImGui::EndPopup();
		}
	}
	else if (!inited && isOnline && canInit)
	{
		unsigned int* sizes = SManager::GetCanvasSize();
		std::cout << "Sizes:" << sizes[0] << "; " << sizes[1] << std::endl;
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