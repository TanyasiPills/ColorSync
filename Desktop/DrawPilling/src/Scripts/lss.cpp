#include "lss.h"
#include <vector>
#include <iostream>
#include "stb_image/stb_image.h"
#include "Texture.h"


//
// This file is a wrapper aroun ImGui so that calling ui objects and costumizing them wont take one houndred line per UI element
//

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
	ImVec4(0.0902f, 0.0902f, 0.2471f, 1.0f),
	ImVec4(0.0627f, 0.0627f, 0.1451f, 1.0f),
	ImVec4(0.1412f, 0.1412f, 0.3529f, 1.0f),
	ImVec4(0.1843f, 0.1843f, 0.4353f, 1.0f),
	ImVec4(0.478f, 0.455f, 0.651f, 1.0f),
	ImVec4(0.647f, 0.627f, 0.831f, 1.0f),
	ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
	ImVec4(0.1059f, 0.1059f, 0.2980f, 1.0f),
	ImVec4(0.647f, 0.627f, 0.831f, 0.4f),
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


	ImGui::GetStyle().Colors[ImGuiCol_Header] = colorArray[HeavyHighlight];
	ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = colorArray[LowHighlight];
	ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = colorArray[HeavyHighlight];

	ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = colorArray[HeavyHighlight];
	ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = colorArray[HeavyHighlight];

	ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = colorArray[LowHighlight];
	ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = colorArray[Border];

	ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.6f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = colorArray[InputBg];
	ImGui::GetStyle().Colors[ImGuiCol_TextDisabled] = colorArray[Hint];

	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.2431f, 0.2431f, 0.5961f, 1.0f);

	window = windowIn;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	VH = (float)screenHeight / 100;
	VW = (float)screenWidth / 100;
	for (int x = VH; x < 10*(int)VH; x += VH/2)
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

void Lss::End() {
	prevType = -1;
	if (haveFont) ResetFont();
}


void Lss::SetColor(int region, int colorToSet) {
	ImGui::GetStyle().Colors[regionArray[region]] = colorArray[colorToSet];
}

void Lss::SetFontSize(float size) {
	haveFont = true;
	int type = (size / VH * 2) - 1;
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


void Center(float itemWidth) {
	float width = ImGui::GetWindowWidth();
	ImGui::SetCursorPosX((width - itemWidth)*0.5f);
}

void CenterWindow() {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
}


//windows
void Lss::Child(std::string name, ImVec2 size, bool border, int flags, ImGuiWindowFlags windowFlags) {
	if (flags & Centered && size.x > 0) Center(size.x);
	if (flags & Rounded) {
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
	}
	if (flags & Bordering) {
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, Lss::VH / 6);
	}
	if (flags & Trans) {
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, colorArray[Transparent]);
	}
	if(size.x == 0 && size.y == 0) ImGui::BeginChild(name.c_str(), ImVec2(0,0), border, windowFlags);
	else ImGui::BeginChild(name.c_str(), size, border, windowFlags);
	if (flags & Rounded) {
		ImGui::PopStyleVar();
	}
	if (flags & Bordering) {
		ImGui::PopStyleVar();
	}
	if (flags & Trans) {
		ImGui::PopStyleColor();
	}
}

bool Lss::Modal(std::string label, bool* open, ImVec2 size, int flags, int windowFlags)
{
	if (*open) {
		ImGui::OpenPopup(label.c_str());
		ImGui::SetNextWindowSize(size);
		if (flags & Trans) {
			ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, colorArray[Transparent]);
		}
		if (flags & Centered) {
			CenterWindow();
		}
		if (flags & Bordering) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Lss::VH / 6);
		}
		if (flags & Rounded) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
		}
	}
	bool isOpen = ImGui::BeginPopupModal(label.c_str(), open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | windowFlags);

	if (*open) {
		if (flags & Trans) {
			ImGui::PopStyleColor();
		}
		if (flags & Bordering) {
			ImGui::PopStyleVar();
		}
		if (flags & Rounded) {
			ImGui::PopStyleVar();
		}
	}
	return isOpen;
}


//basic elements
bool Lss::Button(std::string textIn, ImVec2 size, float textSizeIn, int flags) {
	SetFontSize(textSizeIn);

	float originalRounding;
	ImVec4 originalBtnBgColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
	float textSize = ImGui::CalcTextSize(textIn.c_str()).x+ (ImGui::GetStyle().FramePadding.x)*2.0f;

	bool centered = (flags & Centered) != 0;
	bool rounded = (flags & Rounded) != 0;

	if (rounded) {
		originalRounding = ImGui::GetStyle().FrameRounding;
		ImGui::GetStyle().FrameRounding = size.y/4;
	}
	if (flags & Invisible) {
		originalBtnBgColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
	}

	if (centered) Center(size.x);
	if (flags & SameLine) ImGui::SameLine();

	ImGuiStyle& style = ImGui::GetStyle();
	float originalPaddingY = style.FramePadding.y;
	style.FramePadding.y -= 4.0f;

	bool pressed = ImGui::Button(textIn.c_str(), size);

	style.FramePadding.y = originalPaddingY;

	if (rounded) ImGui::GetStyle().FrameRounding = originalRounding;
	if (flags & Invisible) ImGui::GetStyle().Colors[ImGuiCol_Button] = originalBtnBgColor;

	return pressed;
}

void Lss::Text(std::string textIn, float size, int flags) {
	SetFontSize(size);

	ImVec2 textSize = ImGui::CalcTextSize(textIn.c_str());
	if (flags & Centered) {
		Center(textSize.x);
	}
	if (flags & SameLine) ImGui::SameLine();
	ImGui::Text(textIn.c_str());
}

void Lss::Image(GLuint texture, ImVec2 size, int flags, ImVec2 min, ImVec2 max, ImVec4 colorTint) {
	if (texture == -1) return;
	if (size.x == -1) size = ImGui::GetContentRegionAvail();
	if (flags & Centered) Center(size.x);
	if (flags & SameLine) ImGui::SameLine();

	if (flags & Rounded) {
		ImVec2 p_min = ImGui::GetCursorScreenPos();
		ImVec2 p_max = ImVec2(p_min.x + size.x, p_min.y + size.y);
		ImGui::GetWindowDrawList()->AddImageRounded(texture, p_min, p_max, min, max, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), size.y * 0.5f);
		ImGui::Dummy(size);
	}
	else {
		if(colorTint.w != 0) ImGui::Image(texture, size, min, max, colorTint);
		else ImGui::Image(texture, size, min, max);
	}
}

bool Lss::InputText(std::string label, char* buffer, size_t buffer_size, ImVec2 size, int flags, int inputFlags, int maxWidth, std::string hint) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	if(!(flags & MultiLine))SetFontSize(size.y/2);
	if(maxWidth == 0) ImGui::SetNextItemWidth(size.x - (size.y));
	else ImGui::SetNextItemWidth(-FLT_MIN);

	if (flags & Centered) Center(size.x);

	if (flags & SameLine) ImGui::SameLine();

	if (flags & Rounded) {
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
		if(flags & MultiLine)
			draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, size.y / 8);
		else
			draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, size.y / 4);
		
		if (flags & MultiLine)
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, (size.y - ImGui::GetTextLineHeight()) / 8));
		else
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, (size.y - ImGui::GetTextLineHeight()) / 2));

		ImGui::PushStyleColor(ImGuiCol_FrameBg, colorArray[Transparent]);

		if (flags & MultiLine)
			ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.y / 8), pos.y));
		else
			ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.y / 2), pos.y));
	}
	if (flags & Trans) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, colorArray[Transparent]);
	}
	bool modified;
	if (flags & MultiLine) {
		size.x -= size.x * 0.05f;
		ImGui::InputTextMultiline(("##" + label).c_str(), buffer, buffer_size, size, inputFlags);
	}
	else {
		if (!hint.empty()) modified = ImGui::InputTextWithHint(("##" + label).c_str(), hint.c_str(), buffer, buffer_size, inputFlags);
		else modified = ImGui::InputText(("##" + label).c_str(), buffer, buffer_size, inputFlags);
	}

	if (flags & Rounded) {
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(1);
	}
	if (flags & Trans) {
		ImGui::PopStyleColor(1);
	}
	return modified;
}
bool Lss::InputInt(std::string label, int* value, ImVec2 size, int flags, int inputFlags) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	if (flags & Centered) Center(size.x);

	SetFontSize(size.y);
	ImGui::SetNextItemWidth(size.x - (size.y));

	if (flags & SameLine) ImGui::SameLine();

	if (flags & Rounded) {
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
		draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, size.y / 4);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, (size.y - ImGui::GetTextLineHeight()) / 2));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, colorArray[Transparent]);
		ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.y / 2), pos.y));
	}


	bool modified = ImGui::InputInt(("##" + label).c_str(), value, 0, 0, inputFlags);

	if (flags & Rounded) {
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(1);
	}

	return modified;
}

void Lss::Separator(float thickness, float width, int color, int flags) {
	if (flags & Centered) Center(width);

	if (flags & SameLine) ImGui::SameLine();

	ImVec2 p1 = ImGui::GetCursorScreenPos();
	ImVec2 p2;

	if (width > 0.0f) p2 = ImVec2(p1.x + width, p1.y);
	else p2 = ImVec2(p1.x + ImGui::GetContentRegionAvail().x, p1.y);

	ImGui::GetWindowDrawList()->AddLine(p1, ImVec2(p2.x, p1.y), ImGui::ColorConvertFloat4ToU32(colorArray[color]), thickness);

	ImGui::Dummy(ImVec2(0.0f, thickness));
}


//functions for moving elemnts to correct positions
void Lss::Left(float distance) {
	float currentPosX = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(currentPosX + distance);
}
void Lss::Right(float distance) {
	float currentPosX = ImGui::GetCursorPosX(); 
	ImGui::SetCursorPosX(currentPosX - distance); 
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

void Lss::LeftTop(float leftIn, float topIn)
{
	Lss::Left(leftIn);
	Lss::Top(topIn);
}
void Lss::LeftBottom(float leftIn, float bottomIn)
{
	Lss::Left(leftIn);
	Lss::Bottom(bottomIn);
}
void Lss::RightTop(float rightIn, float topIn)
{
	Lss::Right(rightIn);
	Lss::Top(topIn);
}
void Lss::RightBottom(float rightIn, float bottomIn)
{
	Lss::Right(rightIn);
	Lss::Bottom(bottomIn);
}

void Lss::Back() {
	ImGui::SetCursorPos(originalCursorPosition);
	originalCursorPosition.x = -FLT_MAX;
}

//bound checking
bool Lss::InBound(ImVec2 pos, ImVec2 start, ImVec2 size)
{
	return !(pos.x < start.x || pos.x >(start.x + size.x) ||
			pos.y < start.y || pos.y >(start.y + size.y));
}