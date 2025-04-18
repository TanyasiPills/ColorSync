#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <math.h>

#include "NewRenderer.h"
#include "Texture.h"
#include "CallBacks.h"
#include "DrawUI.h"
#include "lss.h"
#include "SocksManager.h"
#include "FileExplorer.h"
#include "NewDraw.h"


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
RenderData cursor;
RenderData messageCursor;

CanvasData dataForCanvas;


float cursorRadius = 0.01;
float zoomRatio = 1.0f;
float initialCanvasRatio[2] = { 1.0f,1.0f };
float canvasRatio[2] = {1,1};
float identityRatio[2] = {1,1};
float offset[2] = {0,0};
float cursorScale[3] = {1.0,1.0, 1};
float identityOffset[2] = {0,0};
float prevPos[2] = { 0,0 };
float prevPrevPos[2] = { 0,0 };
unsigned int canvasSize[2] = {1,1};
unsigned int currentBrush = 0;


double currentPos[2] = { 0.0, 0.0};

std::vector<Position> drawPositions;
Position sentOffset;
float color[3];
float sentBrushSize;


bool online = false;


void NewRenderer::Init(GLFWwindow* windowIn)
{
	SetMainThreadCallback([this](const DrawMessage& msg) {
		taskQueue.push(msg);
		});

	window = windowIn;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
}
void NewRenderer::InitBrushes()
{
	brushes.clear();

	RenderData cursorBrush;
	NewDraw::InitBrush(cursorBrush, cursorRadius, "Resources/Textures/cursor.png");
	brushes.push_back(cursorBrush);

	RenderData penBrush;
	NewDraw::InitBrush(penBrush, cursorRadius);
	brushes.push_back(penBrush);

	RenderData eraseBrush;
	NewDraw::InitBrush(eraseBrush, cursorRadius, "Resources/Textures/erase.png");
	brushes.push_back(eraseBrush);

	RenderData airBrush;
	NewDraw::InitBrush(airBrush, cursorRadius, "Resources/Textures/airBrush.png");
	brushes.push_back(airBrush);

	RenderData waterBrush;
	NewDraw::InitBrush(waterBrush, cursorRadius, "Resources/Textures/waterBrush.png");
	brushes.push_back(waterBrush);

	RenderData charCoalBrush;
	NewDraw::InitBrush(charCoalBrush, cursorRadius, "Resources/Textures/charCoalBrush.png");
	brushes.push_back(charCoalBrush);

	NewDraw::InitBrush(messageCursor, cursorRadius);

	cursor = brushes[0];
}
void NewRenderer::SetDrawData(unsigned int& canvasWidthIn, unsigned int& canvasHeightIn)
{
	canvasSize[0] = canvasWidthIn;
	canvasSize[1] = canvasHeightIn;

	InitBrushes();

	dataForCanvas = NewDraw::initCanvas(canvasSize[0], canvasSize[1], window);

	initialCanvasRatio[0] = dataForCanvas.canvasX;
	initialCanvasRatio[1] = dataForCanvas.canvasY;
	canvasRatio[0] = dataForCanvas.canvasX;
	canvasRatio[1] = dataForCanvas.canvasY;
}
void NewRenderer::InitNewCanvas()
{
	nodes.clear();
	folders.clear();
	layers.clear();
	nextFreeNodeIndex = 0;

	nodes[nextFreeNodeIndex] = std::make_unique<Folder>("Root", nextFreeNodeIndex);
	folders.push_back(nextFreeNodeIndex);
	nextFreeNodeIndex++;

	nodes[nextFreeNodeIndex] = std::make_unique<Layer>("BackGround", nextFreeNodeIndex, dataForCanvas.data);
	layers.push_back(nextFreeNodeIndex);
	currentNode = nextFreeNodeIndex;
	nextFreeNodeIndex++;
	dynamic_cast<Folder*>(nodes[0].get())->AddChild(currentNode);

	int index = nextFreeNodeIndex;
	layers.push_back(index);
	RenderData createdLayer;
	NewDraw::initLayer(createdLayer);
	nodes[index] = std::make_unique<Layer>("Main", index, createdLayer);
	nextFreeNodeIndex++;
	dynamic_cast<Folder*>(nodes[0].get())->AddChild(index);

	if (online) SManager::ProcessHistory();

	inited = true;
}


void NewRenderer::SetOnline(bool value){
	online = value;
}
bool NewRenderer::GetOnline() {
	return online;
}

unsigned int* NewRenderer::GetCanvasSize()
{
	std::cout << "width: " << canvasSize[0] << ", height: " << canvasSize[1];

	return canvasSize;
}
void NewRenderer::SetCanvasSize(unsigned int* sizes)
{
	canvasSize[0] = sizes[0];
	canvasSize[1] = sizes[1];
}


int NewRenderer::GetParent(int& id)
{
	for (int folderIndex : folders)
	{
		Folder* folder = dynamic_cast<Folder*>(nodes[folderIndex].get());

		if (folder && std::find(folder->childrenIds.begin(), folder->childrenIds.end(), id) != folder->childrenIds.end()) {
			return folderIndex;
		}
	}
	return 0;
}
int NewRenderer::CreateLayer(int& parent)
{
	int index = nextFreeNodeIndex;
	layers.push_back(index);
	RenderData createdLayer;
	NewDraw::initLayer(createdLayer);
	nodes[index] = std::make_unique<Layer>("NewLayer"+std::to_string(layers.size()+1), index, createdLayer);
	nextFreeNodeIndex++;
	dynamic_cast<Folder*>(nodes[parent].get())->AddChild(index);
	NewDraw::MoveCanvas(createdLayer, canvasRatio, offset);
	return index;
}
int NewRenderer::CreateFolder(int& parent)
{
	int index = nextFreeNodeIndex;
	nodes[index] = std::make_unique<Folder>("Folder", index);
	folders.push_back(index);
	dynamic_cast<Folder*>(nodes[parent].get())->AddChild(index);
	return index;
}
void NewRenderer::RemoveLayer(int& index)
{
	auto it = std::find(layers.begin(), layers.end(), index);
	if (it != layers.end())
	{
		layers.erase(it);
		nodes.erase(index);
	}
}
void NewRenderer::RemoveFolder(int& index)
{
	auto it = std::find(folders.begin(), folders.end(), index);
	if (it != folders.end())
	{
		folders.erase(it);
		nodes.erase(index);
	}
}


void NewRenderer::ChangeBrush(int index)
{
	cursor = brushes[index];
	currentBrush = index;
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
	zoomRatio *= scale;
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

void NewRenderer::SetColor(float* colorIn) {
	color[0] = colorIn[0];
	color[1] = colorIn[1];
	color[2] = colorIn[2];
}


//rendering the brushes to the canvas
void NewRenderer::LoadPrevCursor(double* prevIn)
{
	prevPrevPos[0] = prevIn[0];
	prevPrevPos[1] = prevIn[1];

	prevPos[0] = prevIn[0];
	prevPos[1] = prevIn[1];
}
void NewRenderer::RenderCursorToCanvas()
{
	if (recieving) return;
	if (Layer* layerPtr = dynamic_cast<Layer*>(nodes[currentNode].get())) {
		RenderData& layer = layerPtr->data;
		if (layers[0] == currentNode) return;

		glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);
		glViewport(0, 0, canvasSize[0], canvasSize[1]);
		double* pos = Callback::GlCursorPosition();

		switch (tool)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: { // draw
			if (tool == 2) {
				cursor = brushes[1];
				cursor.shader->Bind();
				cursor.shader->SetUniform4f("Kolor", 0.0f, 0.0f, 0.0f, 0.0f);
				glBlendFunc(GL_ONE, GL_ZERO);
			}
			float dx = pos[0] - prevPos[0];
			float dy = pos[1] - prevPos[1];
			dx *= canvasSize[0];
			dy *= canvasSize[1];
			float distance = std::sqrt(dx * dx + dy * dy);

			int num_samples = 1 + distance / (cursorRadius * 100);

			float ctrlX = 2 * prevPos[0] - 0.5f * (prevPrevPos[0] + pos[0]);
			float ctrlY = 2 * prevPos[1] - 0.5f * (prevPrevPos[1] + pos[1]);

			drawPositions.push_back(Position(pos[0], pos[1]));

			for (int i = 0; i < num_samples; ++i) {
				float t = static_cast<float>(i) / num_samples;

				float vx = (1 - t) * (1 - t) * prevPrevPos[0] + 2 * (1 - t) * t * ctrlX + t * t * pos[0];
				float vy = (1 - t) * (1 - t) * prevPrevPos[1] + 2 * (1 - t) * t * ctrlY + t * t * pos[1];

				double tmp[2] = { vx, vy };
				NewDraw::BrushToPosition(window, cursor, cursorRadius, canvasRatio, offset, cursorScale, tmp);
				Draw(cursor);
			}
			prevPrevPos[0] = prevPos[0];
			prevPrevPos[1] = prevPos[1];
			prevPos[0] = pos[0];
			prevPos[1] = pos[1];
			if (tool == 2) {
				cursor.shader->SetUniform4f("Kolor", color[0], color[1], color[2], 1.0f);
				cursor = brushes[2];
				cursor.shader->Bind();
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			}
		} break;
		case 6: { // fill
			ImVec4 colorIn(color[0], color[1], color[2], 1.0f);
			NewDraw::Fill(layerPtr, pos[0], pos[1], colorIn);
		} break;
		case 201: { // colorpicker[0]
			int pixelX = static_cast<int>((pos[0]) / canvasRatio[0] * canvasSize[0]);
			int pixelY = static_cast<int>((pos[1]) / canvasRatio[1] * canvasSize[0]);

			unsigned char pixelColor[4];
			glReadPixels(pixelX, pixelY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);

			ImVec4 pickedColor(pixelColor[0] / 255.0f,
				pixelColor[1] / 255.0f,
				pixelColor[2] / 255.0f,
				pixelColor[3] / 255.0f);

			color[0] = pickedColor.x;
			color[1] = pickedColor.y;
			color[2] = pickedColor.z;

			unsigned char whiteColor[4] = { 255, 255, 255, 255 };

			glRasterPos2i(pixelX, pixelY);
			glDrawPixels(1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteColor);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			DrawUI::SetColor(color);
		} break;
		default:
			break;
		}

		layer.texture->Bind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
	else {
		std::cerr << "Error: Node is not of type Layer!" << std::endl;
	}
}


//sending & recieving draw data over the network
void NewRenderer::SetDrawDataJa()
{
	sentBrushSize = cursorRadius;
	sentOffset.x = offset[0];
	sentOffset.y = offset[1];
}
void NewRenderer::SendDraw() 
{
	DrawMessage msg;
	msg.type = 0;
	msg.layer = currentNode;
	msg.brush = currentBrush;
	msg.size = cursorRadius;
	msg.positions = drawPositions;
	msg.offset = Position(offset[0], offset[1]);
	msg.color[0] = color[0];
	msg.color[1] = color[1];
	msg.color[2] = color[2];
	msg.ratio.x = canvasRatio[0];
	msg.ratio.y = canvasRatio[1];
	msg.cursorScale[0] = cursorScale[0];
	msg.cursorScale[1] = cursorScale[1];
	msg.cursorScale[2] = cursorScale[2];
	SManager::SendAction(msg);

	drawPositions.clear();
}
void NewRenderer::RenderDrawMessage(const DrawMessage& drawMessage)
{
	try {
		if (Layer* layerPtr = dynamic_cast<Layer*>(nodes[drawMessage.layer].get())) {
			RenderData& layer = layerPtr->data;
			if (layers[0] == drawMessage.layer) return;
			if (drawMessage.positions.size() < 1) return;

			glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);
			glViewport(0, 0, canvasSize[0], canvasSize[1]);

			double preves[2] = { drawMessage.positions[0].x, drawMessage.positions[0].y };
			LoadPrevCursor(preves);

			messageCursor = brushes[drawMessage.brush];
			tool = drawMessage.brush;
			int posIndex = 0;
			Position pos = drawMessage.positions[0];

			switch (tool) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5: { // draw
				if (tool == 2) {
					messageCursor = brushes[1];
					messageCursor.shader->Bind();
					messageCursor.shader->SetUniform4f("Kolor", 0.0f, 0.0f, 0.0f, 0.0f);
					glBlendFunc(GL_ONE, GL_ZERO);
				}
				else {
					messageCursor.shader->Bind();
					messageCursor.shader->SetUniform4f("Kolor", drawMessage.color[0], drawMessage.color[1], drawMessage.color[2], 1.0f);
				}
				while (posIndex < drawMessage.positions.size()) {
					pos = drawMessage.positions[posIndex++];

					float messageOffset[2] = { drawMessage.offset.x, drawMessage.offset.y };
					float messageCursorscale[3] = { drawMessage.cursorScale[0], drawMessage.cursorScale[1], drawMessage.cursorScale[2] };
					float dx = pos.x - prevPos[0];
					float dy = pos.y - prevPos[1];
					dx *= canvasSize[0];
					dy *= canvasSize[1];
					float distance = std::sqrt(dx * dx + dy * dy);

					float radius = drawMessage.size;

					float messageCanvasRatio[2] = { drawMessage.ratio.x, drawMessage.ratio.y };

					int num_samples = 1 + distance / (radius * 100);

					float ctrlX = 2 * prevPos[0] - 0.5f * (prevPrevPos[0] + pos.x);
					float ctrlY = 2 * prevPos[1] - 0.5f * (prevPrevPos[1] + pos.y);

					for (int i = 0; i < num_samples; ++i) {
						float t = static_cast<float>(i) / num_samples;

						float vx = (1 - t) * (1 - t) * prevPrevPos[0] + 2 * (1 - t) * t * ctrlX + t * t * pos.x;
						float vy = (1 - t) * (1 - t) * prevPrevPos[1] + 2 * (1 - t) * t * ctrlY + t * t * pos.y;

						double tmp[2] = { vx, vy };
						NewDraw::BrushToPosition(window, messageCursor, radius, messageCanvasRatio, messageOffset, messageCursorscale, tmp);
						Draw(messageCursor);
					}
					prevPrevPos[0] = prevPos[0];
					prevPrevPos[1] = prevPos[1];
					prevPos[0] = pos.x;
					prevPos[1] = pos.y;
				}

				if (tool == 2) {
					messageCursor.shader->SetUniform4f("Kolor", color[0], color[1], color[2], 1.0f);
					messageCursor = brushes[2];
					messageCursor.shader->Bind();
					glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				}
			} break;
			case 6: { // fill
				ImVec4 colorIn(color[0], color[1], color[2], 1.0f);
				NewDraw::Fill(layerPtr, pos.x, pos.y, colorIn);
			} break;
			case 201: { // colorpicker[0]
				int pixelX = static_cast<int>((pos.x) / canvasRatio[0] * canvasSize[0]);
				int pixelY = static_cast<int>((pos.y) / canvasRatio[1] * canvasSize[0]);

				unsigned char pixelColor[4];
				glReadPixels(pixelX, pixelY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);

				ImVec4 pickedColor(pixelColor[0] / 255.0f,
					pixelColor[1] / 255.0f,
					pixelColor[2] / 255.0f,
					pixelColor[3] / 255.0f);

				color[0] = pickedColor.x;
				color[1] = pickedColor.y;
				color[2] = pickedColor.z;

				unsigned char whiteColor[4] = { 255, 255, 255, 255 };

				glRasterPos2i(pixelX, pixelY);
				glDrawPixels(1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteColor);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				DrawUI::SetColor(color);
			} break;
			default:
				break;
			}

			layer.texture->Bind();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
		}
	}
	catch (...) {
		std::cerr << "cant render drawMessage, bad data" << std::endl;
	}
	recieving = false;
}

void NewRenderer::SendLayerRename(std::string nameIn, int locationIn) {
	NodeRenameMessage msg;
	msg.type = 2;
	msg.name = nameIn;
	msg.location = locationIn;
}


//rendering every action & item
void NewRenderer::Draw(const RenderData& data)
{
	GLCall(data.va->Bind());
	GLCall(data.shader->Bind());
	GLCall(data.texture->Bind());
	GLCall(glDrawElements(GL_TRIANGLES, data.ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}


// render the drawing part
void NewRenderer::RenderLayers()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int item : layers) {
		Layer layer = *dynamic_cast<Layer*>(nodes[item].get());
		if (layer.visible) {
			layer.data.shader->Bind();
			layer.data.shader->SetUniform1f("opacity", layer.opacity / 100.0f);
			Draw(layer.data);
		}
	}
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}
void NewRenderer::RenderCursor()
{
	double* now = Callback::GlCursorPosition();
	float curRad = (tool == 0) ? 0.02f : cursorRadius;
	NewDraw::BrushToPosition(window, cursor, curRad, identityRatio, identityOffset, cursorScale, now);
	Draw(cursor);

}
void RenderImGui(bool& onUIIn)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	onUIIn = ImGui::GetIO().WantCaptureMouse;
	Lss::SetFontSize(2 * Lss::VH);

	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);

	DrawUI::InitWindow();

	DrawUI::DrawMenu();
	DrawUI::PlayerVisualization();
	DrawUI::ColorWindow(cursor);
	DrawUI::SizeWindow(cursorRadius, zoomRatio);
	DrawUI::BrushWindow(window, cursor);
	DrawUI::ServerWindow();
	DrawUI::LayerWindow();
	DrawUI::ChatWindow();

	if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE)
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

	ImGui::PopStyleColor();

	Lss::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


//rendering the social media part
void RenderMenu()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	int width, height;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	int sideWidth = width * (1.0f / 6.0f);
	int mainWidth = width * (4.0f / 6.0f);
	int windowHeight = (float)height;

	int totalWidth = sideWidth + mainWidth + sideWidth;
	mainWidth += (width - totalWidth); 
	

	SocialMedia::LeftSide(0,sideWidth, windowHeight);
	SocialMedia::MainFeed(sideWidth, mainWidth, windowHeight);
	SocialMedia::RightSide(sideWidth+mainWidth, sideWidth, windowHeight);


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void NewRenderer::SwapView(bool isOnline)
{
	isEditor = !isEditor;
	if (isEditor) {
		online = isOnline;
		DrawUI::InitData();
	}
}

void NewRenderer::Clear()
{
	glClearColor(0.188, 0.188, 0.313, 0);
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}
void NewRenderer::Render()
{
	Clear();
	if (isEditor) {
		if (inited) {
			RenderLayers();
			RenderCursor();
			if(DrawUI::canInit) ProcessTasks();
			double* pos = Callback::GlCursorPosition();

			if (online) {
				if (currentPos[0] != pos[0] || currentPos[1] != pos[1]) {
					Position posy;
					posy.x = pos[0];
					posy.y = pos[1];
					SManager::SendPositionMessage(posy);
					currentPos[0] = pos[0];
					currentPos[1] = pos[1];
				}
			}
		}
		RenderImGui(onUI);
	}
	else {
		SocialMedia::ProcessThreads();
		RenderMenu();
	}

	glfwSwapBuffers(window);
}