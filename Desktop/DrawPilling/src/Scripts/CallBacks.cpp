#include "CallBacks.h"
#include "SessionManager.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

double prevCursorPos[2];
float GlCursorPos[2];
NewRenderer* renderer;
int screenWidth, screenHeight;
static float offset[2];
static float windowOffset[2] = {0,0};

bool mouseDown = false, moveCanvas = false;
static float scale = 1.0;
float yRatio = 1.0;
int originalSizeX = 1.0, originalSizeY = 1.0;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset,yoffset);
	if (renderer->onUI) return;

	if (yoffset > 0) {
		offset[0] -= (GlCursorPos[0] - offset[0]) * 0.25;
		offset[1] -= (GlCursorPos[1] - offset[1]) * 0.25;
		renderer->Zoom(1.25, offset);
	}
	else if (yoffset < 0) {
		offset[0] += (GlCursorPos[0] - offset[0]) * 0.2;
		offset[1] += (GlCursorPos[1] - offset[1]) * 0.2;
		renderer->Zoom(0.8, offset);
	}
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) 
{
	glViewport(0, 0, width, height);
	renderer->Clear();
	float x = (float)originalSizeX / width;
	float y = (float)originalSizeY / height;
	if (screenWidth != width) {
		float xOffset = ((float)(width - screenWidth) / screenWidth) * x;
		offset[0] -= xOffset;
	}
	if (screenHeight != height) {
		float yOffset = ((float)(height - screenHeight) / screenHeight) * y;
		offset[1] += yOffset;
	}

	screenWidth = width;
	screenHeight = height;
	
	renderer->OnResize(x, y, offset, yRatio);
	renderer->Render();
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	if (renderer->onUI) return;

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		switch (action)
		{
			case GLFW_PRESS:
				mouseDown = true;
				renderer->LoadPrevCursor(GlCursorPos);
				renderer->RenderCursorToCanvas();
				renderer->SetDrawData();
				break;

			case GLFW_RELEASE:
				mouseDown = false;
				renderer->SendDraw();
				break;
		}
	}
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
	float x = xpos / screenWidth;
	float y = (screenHeight - ypos) / screenHeight;
	GlCursorPos[0] = x * 2 - 1;
	GlCursorPos[1] = y * 2 - 1;

	if (mouseDown) renderer->RenderCursorToCanvas();
	if (moveCanvas) {
		if (prevCursorPos[0] != xpos) {
			float xdiff = ((xpos)-prevCursorPos[0]) / screenWidth;
			offset[0] += xdiff * 2;
			prevCursorPos[0] = xpos;
		}
		if (prevCursorPos[1] != ypos) {
			float ydiff = ((ypos)-prevCursorPos[1]) / screenHeight;
			offset[1] -= ydiff * 2;
			prevCursorPos[1] = ypos;
		}
		renderer->MoveLayers(offset);
	}
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	switch (key)
	{
		case GLFW_KEY_SPACE:
			if (action == GLFW_PRESS) {
				moveCanvas = true;
				glfwGetCursorPos(window, &prevCursorPos[0], &prevCursorPos[1]);
			}
			else if (action == GLFW_RELEASE) 
				moveCanvas = false;
			break;
		case GLFW_KEY_F9:
			if (action == GLFW_PRESS) {
				//WindowManager::ToggleFullscreen(window);
			}
		default:
			break;
	}
}

void onCloseCallback(GLFWwindow* window) {
	std::cout << "Window close event intercepted. Doing necessary cleanup before closing.\n";
	Manager::DisAssembly(window);
}
void Callback::EditorSwapCallBack()
{
	renderer->SwapView();
}

void Callback::Init(GLFWwindow* window, NewRenderer& rendererIn)
{
	renderer = &rendererIn;
	glfwGetWindowSize(window, &originalSizeX, &originalSizeY);
	yRatio = (float)originalSizeX / originalSizeY;
	float xRatio = 1;
	renderer->OnResize(xRatio, xRatio, offset, yRatio);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	glfwSetWindowCloseCallback(window, onCloseCallback);
}

float* Callback::GlCursorPosition()
{
	return GlCursorPos;
}