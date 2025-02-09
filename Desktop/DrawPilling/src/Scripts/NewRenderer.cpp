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
#include "lss.h"
#include "SocksManager.h"
#include "SocialMedia.h"


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
std::vector<int> layers;
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

std::vector<Position> drawPositions;
Position sentOffset;
float color[3];
float sentBrushSize;


void NewRenderer::Init(GLFWwindow* windowIn, unsigned int& canvasWidthIn, unsigned int& canvasHeightIn, int screenWidth, int screenHeight)
{
	SetMainThreadCallback([this](const DrawMessage& msg) {
		taskQueue.push(msg);
	});

	window = windowIn;
	canvasSize[0] = canvasWidthIn;
	canvasSize[1] = canvasHeightIn;
	NewDraw::InitBrush(cursor, cursorRadius);
	CanvasData canvasData = NewDraw::initCanvas(canvasWidthIn, canvasHeightIn);

	initialCanvasRatio[0] = canvasData.canvasX;
	initialCanvasRatio[1] = canvasData.canvasY;
	canvasRatio[0] = canvasData.canvasX;
	canvasRatio[1] = canvasData.canvasY;

	nodes[nextFreeNodeIndex] = std::make_unique<Folder>("Root", nextFreeNodeIndex);
	nextFreeNodeIndex++;
	nodes[nextFreeNodeIndex] = std::make_unique<Layer>("Main", nextFreeNodeIndex, canvasData.data);
	layers.push_back(nextFreeNodeIndex);
	currentNode = nextFreeNodeIndex;
	nextFreeNodeIndex++;
	dynamic_cast<Folder*>(nodes[0].get())->AddChild(currentNode);

	nodes[nextFreeNodeIndex] = std::make_unique<Folder>("Folder", nextFreeNodeIndex);
	dynamic_cast<Folder*>(nodes[0].get())->AddChild(nextFreeNodeIndex);
	int folder = nextFreeNodeIndex;
	nextFreeNodeIndex++;

	RenderData layerTwo;
	NewDraw::initLayer(layerTwo, canvasRatio[0], canvasRatio[1]);
	nodes[nextFreeNodeIndex] = std::make_unique<Layer>("Not main", nextFreeNodeIndex, layerTwo);
	layers.push_back(nextFreeNodeIndex);
	dynamic_cast<Folder*>(nodes[folder].get())->AddChild(nextFreeNodeIndex);
	nextFreeNodeIndex++;
}

void NewRenderer::AddLayer(std::string name, int location) 
{
	RenderData layer;
	NewDraw::initLayer(layer, canvasRatio[0], canvasRatio[1]);
	nodes[nextFreeNodeIndex] = std::make_unique<Layer>(name, nextFreeNodeIndex, layer);
	layers.push_back(nextFreeNodeIndex);
	dynamic_cast<Folder*>(nodes[location].get())->AddChild(nextFreeNodeIndex);
	nextFreeNodeIndex++;
}

void NewRenderer::AddFolder(std::string name, int location)
{
	nodes[nextFreeNodeIndex] = std::make_unique<Folder>(name, nextFreeNodeIndex);
	dynamic_cast<Folder*>(nodes[location].get())->AddChild(nextFreeNodeIndex);
	nextFreeNodeIndex++;
}

void NewRenderer::MoveLayers(static float* offsetIn)
{
	offset[0] = offsetIn[0];
	offset[1] = offsetIn[1];
	for (int item : layers) {
		Layer layer = *dynamic_cast<Layer*>(nodes[item].get());
		NewDraw::MoveCanvas(layer.data, canvasRatio, offset);
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

	for (int item : layers) {
		Layer layer = *dynamic_cast<Layer*>(nodes[item].get());
		NewDraw::MoveCanvas(layer.data, canvasRatio, offset);
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

	for (int item : layers) {
		Layer layer = *dynamic_cast<Layer*>(nodes[item].get());
		NewDraw::MoveCanvas(layer.data, canvasRatio, offset);
	}
}

void NewRenderer::LoadPrevCursor(float* prevIn)
{
	prevPos[0] = prevIn[0];
	prevPos[1] = prevIn[1];
}

void NewRenderer::RenderCursorToCanvas(int currentLayerIn)
{
	if (recieving) return;
	if (Layer* layerPtr = dynamic_cast<Layer*>(nodes[currentNode].get())) {
		RenderData& layer = layerPtr->data;
		glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);
		glViewport(0, 0, canvasSize[0], canvasSize[1]);

		float* pos = Callback::GlCursorPosition();
		float dx = pos[0] - prevPos[0];
		float dy = pos[1] - prevPos[1];
		dx *= canvasSize[0];
		dy *= canvasSize[1];
		float distance = std::sqrt(dx * dx + dy * dy);
		int num_samples = std::min(static_cast<int>(std::exp(distance / (cursorRadius * canvasSize[0]))), 100);
		if (num_samples < 1) num_samples = 100;
	
		for (int i = 0; i < num_samples; ++i) {
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
		layer.texture->Bind();
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
	else {
		std::cerr << "Error: Node is not of type Layer!" << std::endl;
	}
}

void NewRenderer::SendDraw() 
{
	DrawMessage msg;
	msg.type = 0;
	msg.layer = 1;
	msg.brush = 2;
	msg.size = sentBrushSize;
	msg.positions = drawPositions;
	msg.offset = sentOffset;
	msg.color[0] = color[0];
	msg.color[1] = color[1];
	msg.color[2] = color[2];
	SManager::SendAction(msg);

	drawPositions.clear();
}

void NewRenderer::SendLayerRename(std::string nameIn, int locationIn) {
	NodeRenameMessage msg;
	msg.type = 2;
	msg.name = nameIn;
	msg.location = locationIn;
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

void NewRenderer::RenderDrawMessage(const DrawMessage& drawMessage)
{
	try{
		if (Layer* layerPtr = dynamic_cast<Layer*>(nodes[drawMessage.layer].get()))
		{
			RenderData& layer = layerPtr->data;
			glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);
			glViewport(0, 0, canvasSize[0], canvasSize[1]);
			cursor.shader->SetUniform3f("Kolor", drawMessage.color[0], drawMessage.color[1], drawMessage.color[2]);
			Position offs = drawMessage.offset;
			float offse[2] = { offs.x, offs.y };
			float radius = drawMessage.size;
			for (size_t i = 0; i < drawMessage.positions.size(); i++)
			{
				Position pos = drawMessage.positions[i];
				float tmp[2] = { pos.x, pos.y };
				NewDraw::BrushToPosition(window, cursor, radius, canvasRatio, offse, cursorScale, tmp,1);
				Draw(cursor);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			layer.texture->Bind();
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
		}
		else {
			std::cerr << "sent layer not found" << std::endl;
		}
	}
	catch (...){
		std::cerr << "cant render drawMessage, bad data" << std::endl;
	}
	recieving = false;
}

void NewRenderer::RenderLayers()
{
	for (int item : layers) {
		Layer layer = *dynamic_cast<Layer*>(nodes[item].get());
		if (layer.visible) {
			Draw(layer.data);
		}
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
	

	SocialMedia::LeftSide(0,sideWidth, windowHeight);
	SocialMedia::MainFeed(sideWidth, mainWidth, windowHeight);
	SocialMedia::RightSide(sideWidth+mainWidth, sideWidth, windowHeight);


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