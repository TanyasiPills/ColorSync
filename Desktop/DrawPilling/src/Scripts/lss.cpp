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

	ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.6f);


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

void CenterWindow() {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
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
	if(size.x == 0 && size.y == 0) ImGui::BeginChild(name.c_str(), ImVec2(0,0), border, windowFlags);
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
		ImGui::GetStyle().FrameRounding = size.y/2;
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
	if (flags & Rounded) {
		ImVec2 p_min = ImGui::GetCursorScreenPos();
		ImVec2 p_max = ImVec2(p_min.x + size.x, p_min.y + size.y);
		ImGui::GetWindowDrawList()->AddImageRounded(texture, p_min, p_max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), size.y * 0.5f);
		ImGui::Dummy(size);
	}
	else {
		ImGui::Image(texture, size);
	}
}
bool Lss::InputText(std::string label, char* buffer, size_t buffer_size, ImVec2 size, int flags, int inputFlags) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	if (flags & Centered) Center(size.x);

	SetFontSize(size.y);
	ImGui::SetNextItemWidth(size.x - (size.y));

	if (flags & Rounded) {
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
		draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, size.y / 2);
		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, (size.y - ImGui::GetTextLineHeight()) / 2));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
		ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.y / 2), pos.y));
	}


	bool modified = ImGui::InputText(("##"+label).c_str(), buffer, buffer_size, inputFlags);

	if (flags & Rounded) {
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(1);
	}

	return modified;
}

bool Lss::InputInt(std::string label, int* value, ImVec2 size, int flags, int inputFlags) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	if (flags & Centered) Center(size.x);

	SetFontSize(size.y);
	ImGui::SetNextItemWidth(size.x - (size.y));

	if (flags & Rounded) {
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
		draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, size.y / 2);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, (size.y - ImGui::GetTextLineHeight()) / 2));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
		ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.y / 2), pos.y));
	}


	bool modified = ImGui::InputInt(("##" + label).c_str(), value, 0, 0, inputFlags);

	if (flags & Rounded) {
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(1);
	}

	return modified;
}

bool Lss::Modal(std::string label, bool* open, ImVec2 size, int flags,int windowFlags)
{
	if (*open) {
		ImGui::OpenPopup(label.c_str());
		ImGui::SetNextWindowSize(size);
		if (flags & Centered) {
			CenterWindow();
		}
	}
	return ImGui::BeginPopupModal(label.c_str(), open, ImGuiWindowFlags_NoResize);
}


void Lss::Separator(float thickness, int color) {
	ImVec2 p1 = ImGui::GetCursorScreenPos();
	ImVec2 p2 = ImVec2(p1.x + ImGui::GetContentRegionAvail().x, p1.y);

	ImGui::GetWindowDrawList()->AddLine(p1, ImVec2(p2.x, p1.y), ImGui::ColorConvertFloat4ToU32(colorArray[color]), thickness);

	ImGui::Dummy(ImVec2(0.0f, thickness));
}


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
void Lss::Back() {
	ImGui::SetCursorPos(originalCursorPosition);
	originalCursorPosition.x = -FLT_MAX;
}


void Lss::End() {
	prevType = -1;
	if (haveFont) ResetFont();
}

bool Lss::InBound(ImVec2 pos, ImVec2 start, ImVec2 size)
{
	return !(pos.x < start.x || pos.x >(start.x + size.x) ||
			pos.y < start.y || pos.y >(start.y + size.y));
}