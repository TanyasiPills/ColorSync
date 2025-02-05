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

MyTexture texture;

void Lss::Init(GLFWwindow* windowIn, int screenWidth,  int screenHeight, std::string path)
{
	int width, height, bpp;
	texture.Init("Resources/Textures/fish.jpg");
	window = windowIn;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	VH = (float)screenHeight / 100;
	VW = (float)screenWidth / 100;
	for (int x = VH; x < 10*(int)VH; x += VH)
	{
		fonts.push_back(io.Fonts->AddFontFromFileTTF("Aptos.ttf", x));
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

void Lss::Update() {
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

void Lss::Button(std::string textIn, ImVec2 size, float textSizeIn, int flags) {
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

	ImGui::Button(textIn.c_str(), size);

	if (rounded) ImGui::GetStyle().FrameRounding = originalRounding;
	if (invisible) ImGui::GetStyle().Colors[ImGuiCol_Button] = originalBtnBgColor;

}

void Lss::Left(float distance) {
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(distance, 0));
}

void Lss::Image() {
	ImVec2 childSize = ImGui::GetContentRegionAvail();
	ImGui::Image(texture.GetId(), ImVec2(childSize.x, childSize.x));
}
void Lss::Top(float distance) {
	ImGui::Dummy(ImVec2(0, distance));
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




void Lss::Text(std::string textIn, float size, int flags) {
	SetFontSize(size);

	ImVec2 textSize = ImGui::CalcTextSize(textIn.c_str());
	if (flags & Centered) {
		Center(textSize.x);
	}

	ImGui::Text(textIn.c_str());

	//ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (textSize.y / 2));
}

void Lss::End() {
	prevType = -1;
	if (haveFont) ResetFont();
}



