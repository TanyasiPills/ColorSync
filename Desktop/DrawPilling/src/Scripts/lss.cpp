#include "lss.h"
#include <vector>
#include <iostream>
#include "stb_image/stb_image.h"
#include "Texture.h"

static GLFWwindow* window = nullptr;

bool centered = false;
bool haveFont = false;
int fontPopCount = 0;

ImVec2 originalCursorPosition(-FLT_MAX,0);

std::vector<ImFont*> fonts;

float Lss::VW = 0;
float Lss::VH = 0;

int prevType = -1;

ImVec4 colorArray[] = {
	ImVec4(0.149, 0.149, 0.345, 1.0f),
	ImVec4(0.122, 0.122, 0.298, 1.0f),
	ImVec4(0.208, 0.208, 0.353, 1.0f),
	ImVec4(0.286, 0.282, 0.451, 1.0f),
	ImVec4(0.478, 0.455, 0.651, 1.0f),
	ImVec4(0.647, 0.627, 0.831, 1.0f)
};
int regionArray[] = {
	ImGuiCol_WindowBg,
	ImGuiCol_ChildBg,
	ImGuiCol_Button,
	ImGuiCol_ButtonHovered,
	ImGuiCol_Border,
	ImGuiCol_Text
};

void Lss::Init(GLFWwindow* windowIn, int screenWidth,  int screenHeight)
{
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = colorArray[Background];
	ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = colorArray[ContainerBackground];
	ImGui::GetStyle().Colors[ImGuiCol_Button] = colorArray[LowHighlight];
	ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = colorArray[HeavyHighlight];
	ImGui::GetStyle().Colors[ImGuiCol_Border] = colorArray[Border];
	ImGui::GetStyle().Colors[ImGuiCol_Text] = colorArray[Font];

	window = windowIn;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	VH = (float)screenHeight / 100;
	VW = (float)screenWidth / 100;
	for (int x = VH; x < 10*(int)VH; x += VH)
	{
		fonts.push_back(io.Fonts->AddFontFromFileTTF("Resources/fonts/Aptos.ttf", x));
	}
}

void ResetFont() {
	for (size_t i = 0; i < fontPopCount; i++)
	{
		ImGui::PopFont();
	}
	fontPopCount = 0;
	haveFont = false;
}

void Lss::SetColor(int region, int colorToSet) {
	ImGui::GetStyle().Colors[regionArray[region]] = colorArray[colorToSet];
}

void Center(float itemWidth) {
	float width = ImGui::GetWindowWidth();
	ImGui::SetCursorPosX((width - itemWidth)*0.5f);
}

void Lss::SetFontSize(float size) {
	haveFont = true;
	int type = (size/VH)-1;
	if (prevType != type) {
		if (type >= 0 && type < fonts.size()) {
			ImGui::PushFont(fonts[type]);
		}
		else {
			if (type < 0) ImGui::PushFont(fonts[0]);
			else ImGui::PushFont(fonts[fonts.size() - 1]);
		}
		prevType = type;
		fontPopCount++;
	}

}

void Lss::Child(std::string name, ImVec2 size, bool border, int flags, ImGuiWindowFlags windowFlags) {
	if (flags & Centered && size.x > 0) Center(size.x);
	if(size.x == 0) ImGui::BeginChild(name.c_str(), ImVec2(0,0), border, windowFlags);
	else ImGui::BeginChild(name.c_str(), size, border, windowFlags);
}

bool Lss::Button(std::string textIn, ImVec2 size, float textSizeIn, int flags) {
	SetFontSize(textSizeIn);

	float originalRounding;
	ImVec4 originalBtnBgColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
	float textSize = ImGui::CalcTextSize(textIn.c_str()).x+ (ImGui::GetStyle().FramePadding.x)*2.0f;

	bool centered = (flags & Centered) != 0;
	bool invisible = (flags & Invisible) != 0;
	bool rounded = (flags & Rounded) != 0;

	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
	if (rounded) {
		originalRounding = ImGui::GetStyle().FrameRounding;
		ImGui::GetStyle().FrameRounding = VH;
	}
	if (invisible) {
		originalBtnBgColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 500));
	if (centered) Center(size.x);
	ImGui::PopStyleVar();

	bool pressed = ImGui::Button(textIn.c_str(), size);

	if (rounded) ImGui::GetStyle().FrameRounding = originalRounding;
	if (invisible) ImGui::GetStyle().Colors[ImGuiCol_Button] = originalBtnBgColor;

	return pressed;
}

void Lss::Text(std::string textIn, float size, int flags) {
	SetFontSize(size);

	ImVec2 textSize = ImGui::CalcTextSize(textIn.c_str());
	if (flags & Centered) {
		Center(textSize.x);
	}

	ImGui::Text(textIn.c_str());
}

void Lss::Image(GLuint texture, ImVec2 size, int flags) {
	if (texture == -1) return;
	if (size.x == -1) size = ImGui::GetContentRegionAvail();
	if (flags & Centered) Center(size.x);
	ImGui::Image(texture, size);
}


void Lss::Left(float distance) {
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(distance, 0));
}

void Lss::Top(float distance) {
	float currentPosY = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(currentPosY + distance);
}
void Lss::Bottom(float distance) {
	if (originalCursorPosition.x == -FLT_MAX) {
		originalCursorPosition = ImGui::GetCursorPos();
		ImGui::SetCursorPosY(originalCursorPosition.y - distance);
	}
}
void Lss::Back() {
	ImGui::SetCursorPos(originalCursorPosition);
	originalCursorPosition.x = -FLT_MAX;
}


void Lss::End() {
	prevType = -1;
	if (haveFont) ResetFont();
}



