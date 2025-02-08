#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <math.h>

#include "NewRenderer.h"
#include "Texture.h"
#include "NewDraw.h"
#include "CallBacks.h"
#include "DrawUI.h"
#include "Menu.h"
#include "lss.h"
#include "SocksManager.h"

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGl Error] (" << error << "):" << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

GLFWwindow* window;
std::vector<Layer> layers;
RenderData cursor;

float cursorRadius = 0.01;
float initialCanvasRatio[2] = { 1.0f,1.0f };
float canvasRatio[2] = {1,1};
float identityRatio[2] = {1,1};
float offset[2] = {0,0};
float cursorScale[3] = {1.0,1.0, 1};
float identityOffset[2] = {0,0};
float prevPos[2] = { 0,0 };
unsigned int canvasSize[2] = {1,1};

unsigned int fbo;

std::vector<Position> drawPositions;
Position sentOffset;
float color[3];
float sentBrushSize;


void NewRenderer::Init(GLFWwindow* windowIn, unsigned int& canvasWidthIn, unsigned int& canvasHeightIn, int screenWidth, int screenHeight)
{
	glGenFramebuffers(1, &fbo);
	window = windowIn;
	canvasSize[0] = canvasWidthIn;
	canvasSize[1] = canvasHeightIn;
	NewDraw::InitBrush(cursor, cursorRadius);
	CanvasData canvasData = NewDraw::initCanvas(canvasWidthIn, canvasHeightIn);
	layers.push_back(Layer("Main", lastLayerIndex, canvasData.data));
	lastLayerIndex++;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvasData.data.texture->GetId(), 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	initialCanvasRatio[0] = canvasData.canvasX;
	initialCanvasRatio[1] = canvasData.canvasY;
	canvasRatio[0] = canvasData.canvasX;
	canvasRatio[1] = canvasData.canvasY;
}

void NewRenderer::MoveLayers(static float* offsetIn)
{
	offset[0] = offsetIn[0];
	offset[1] = offsetIn[1];
	for (Layer& item : layers) {
		NewDraw::MoveCanvas(item.data, canvasRatio, offset);
	}
}
void NewRenderer::Zoom(static float scale, static float* offsetIn)
{
	canvasRatio[0] *= scale;
	canvasRatio[1] *= scale;
	initialCanvasRatio[0] *= scale;
	initialCanvasRatio[1] *= scale;
	offset[0] = offsetIn[0];
	offset[1] = offsetIn[1];

	for (Layer& item : layers) {
		NewDraw::MoveCanvas(item.data, canvasRatio, offset);
	}

	cursorRadius *= scale;
}

void NewRenderer::OnResize(float& x, float& y, float* offsetIn, float& yRatio) {
	cursorScale[0] = x;
	cursorScale[1] = y * yRatio;
	canvasRatio[0] = x * initialCanvasRatio[0];
	canvasRatio[1] = y * initialCanvasRatio[1];

	offset[0] = offsetIn[0];
	offset[1] = offsetIn[1];

	for (Layer& item : layers) {
		NewDraw::MoveCanvas(item.data, canvasRatio, offset);
	}
}

void NewRenderer::LoadPrevCursor(float* prevIn)
{
	prevPos[0] = prevIn[0];
	prevPos[1] = prevIn[1];
}

void NewRenderer::RenderCursorToCanvas(int currentLayer)
{

	if (currentLayer < 0 || currentLayer >= layers.size()) {
		std::cerr << "Invalid layer index!" << std::endl;
		return;
	}

	RenderData& layer = layers[currentLayer].data;

	layer.va->Bind();
	layer.ib->Bind();
	layer.shader->Bind();
	layer.texture->Bind();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, canvasSize[0], canvasSize[1]);

	float* pos = Callback::GlCursorPosition();
	float dx = pos[0] - prevPos[0];
	float dy = pos[1] - prevPos[1];
	dx *= canvasSize[0];
	dy *= canvasSize[1];
	float distance = std::sqrt(dx * dx + dy * dy);
	int num_samples = std::min(static_cast<int>(std::exp(distance / (cursorRadius * canvasSize[0]))), 100);
	if (num_samples < 1) num_samples = 100;
	
	for (int i = 0; i <= num_samples; ++i) {
		float t = static_cast<float>(i) / num_samples;
		float vx = prevPos[0] * (1 - t) + pos[0] * t;
		float vy = prevPos[1] * (1 - t) + pos[1] * t;
		float tmp[2] = { vx, vy };

		drawPositions.push_back(Position(vx, vy));
		
		NewDraw::BrushToPosition(window, cursor, cursorRadius, canvasRatio, offset, cursorScale, tmp);
		Draw(cursor);
	}
	prevPos[0] = pos[0];
	prevPos[1] = pos[1];

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

void NewRenderer::SendDraw() 
{
	DrawMessage msg;
	msg.layer = 1;
	msg.brush = 2;
	msg.size = sentBrushSize;
	msg.positions = drawPositions;
	msg.offset = sentOffset;
	msg.color = color;
	SManager::SendAction(Type::Draw, msg);

	drawPositions.clear();
}

void NewRenderer::SetColor(float* colorIn) {
	color[0] = colorIn[0];
	color[1] = colorIn[1];
	color[2] = colorIn[2];
}

void NewRenderer::SetDrawData()
{
	sentBrushSize = cursorRadius;
	sentOffset.x = offset[0];
	sentOffset.y = offset[1];
}
void NewRenderer::Clear() 
{
	glClearColor(0.188, 0.188, 0.313, 0);
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void NewRenderer::Draw(const RenderData& data)
{
	GLCall(data.va->Bind());
	GLCall(data.shader->Bind());
	GLCall(data.texture->Bind());
	GLCall(glDrawElements(GL_TRIANGLES, data.ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void NewRenderer::RenderLayers()
{
	for (Layer& item : layers) {
		Draw(item.data);
	}
}

void NewRenderer::RenderCursor()
{
	float* now = Callback::GlCursorPosition();
	NewDraw::BrushToPosition(window, cursor, cursorRadius, identityRatio, identityOffset, cursorScale, now);
	Draw(cursor);

}

void RenderImGui(bool& onUIIn)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	onUIIn = ImGui::GetIO().WantCaptureMouse;
	Lss::SetFontSize(2 * Lss::VH);

	bool yes = true;
	ImGui::SetNextWindowViewport(10);
	ImGui::ShowDemoWindow(&yes);

	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);

	DrawUI::ColorWindow(cursor);
	DrawUI::SizeWindow(cursorRadius);
	DrawUI::BrushWindow(window);
	DrawUI::ServerWindow();
	DrawUI::LayerWindow();
	DrawUI::ChatWindow();

	DrawUI::LoginWindow();

	if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE)
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

	ImGui::PopStyleColor();

	Lss::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	/*
	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(backup_current_context);
	*/
}

void RenderMenu()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();



	int width, height;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	int sideWidth = width * (2.0f / 7.0f);
	int mainWidth = width * (3.0f / 7.0f);
	int windowHeight = (float)height;

	int totalWidth = sideWidth + mainWidth + sideWidth;
	mainWidth += (width - totalWidth); 
	

	Menu::LeftSide(0,sideWidth, windowHeight);
	Menu::MainFeed(sideWidth, mainWidth, windowHeight);
	Menu::RightSide(sideWidth+mainWidth, sideWidth, windowHeight);


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void NewRenderer::Render()
{
	Clear();
	RenderLayers();
	RenderCursor();
	RenderImGui(onUI);
	//RenderMenu();

	glfwSwapBuffers(window);
}