#include "lss.h"
#include <vector>

static GLFWwindow* window = nullptr;

bool centered = false;

std::vector<ImFont*> fonts;

float Lss::VW = 0;
float Lss::VH = 0;



void Lss::Init(GLFWwindow* windowIn) {
	window = windowIn;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	Update();
	int i = 0, x = powf(2, i);
	while (x < 540) {
		fonts.push_back(io.Fonts->AddFontFromFileTTF("Aptos.ttf", x));
		x = powf(2, i);
		i++;
	}
}

void Lss::Update() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float wWWidth = width / 100.0;
	float wHHeight = height / 100.0;
	Lss::VW = (wWWidth > 1) ? wWWidth : 1.0f;
	Lss::VH = (wHHeight > 1) ? wHHeight : 1.0f;
}

void Center(float itemWidth) {
	float width = ImGui::GetWindowWidth();
	ImGui::SetCursorPosX((width - itemWidth)*0.5f);
}

void SetFontSize(float size) {
	int type = log2(size);
	if (type >= 0 && type < fonts.size()) {
		ImGui::PushFont(fonts[type]);
	}
	else {
		if(type < 0) ImGui::PushFont(fonts[0]);
		else ImGui::PushFont(fonts[fonts.size()-1]);
	}
	ImGui::GetFont()->FontSize = size;
}

void ResetFont() {
	ImGui::PopFont();
}

void Lss::Button() {

}


void Lss::Text(std::string textIn, float size, bool centeredIn) {
	SetFontSize(size);
	int windowWidth, windowHeight;
	ImVec2 textSize = ImGui::CalcTextSize(textIn.c_str());
	if (centeredIn) {
		Center(textSize.x);
	}
	ImGui::Text(textIn.c_str());
	if (centeredIn)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());
	}
	ResetFont();
}



