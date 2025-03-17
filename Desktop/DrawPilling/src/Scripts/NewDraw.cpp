#include "NewDraw.h"
#include "CallBacks.h"
#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>

unsigned int canvasWidth = 0, canvasHeight = 0;
GLuint floodFillShader;
GLuint texture, activeBuffer, visitedBuffer;

float xScale = 1;
float yScale = 1;

bool AreColorsEqual(const ImVec4& col1, const ImVec4& col2, float epsilon = 0.01f) {
	return fabs(col1.x - col2.x) < epsilon &&
		fabs(col1.y - col2.y) < epsilon &&
		fabs(col1.z - col2.z) < epsilon &&
		fabs(col1.w - col2.w) < epsilon;
}

void fillPositions(float* positions, float xScale = 0.0, float yScale = 0.0, float xPos = 0.0, float yPos = 0.0) {
	positions[0] = (-xScale) + xPos; positions[1] = (-yScale) + yPos; positions[2] = 0.0f; positions[3] = 0.0f;
	positions[4] = xScale + xPos; positions[5] = (-yScale) + yPos; positions[6] = 1.0f; positions[7] = 0.0f;
	positions[8] = xScale + xPos; positions[9] = yScale + yPos; positions[10] = 1.0f; positions[11] = 1.0f;
	positions[12] = (-xScale) + xPos; positions[13] = yScale + yPos; positions[14] = 0.0f; positions[15] = 1.0f;
}

void initData(RenderData& data, float* positions, const char* texture = nullptr, int shaderType = 0, int transparent = 0, std::vector<unsigned char> vectorOfTexture = {})
{

	if (!data.va) {
		data.va = std::make_shared<VertexArray>();
	}
	if (!data.ib) {
		data.ib = std::make_shared<IndexBuffer>();
	}
	if (!data.shader) {
		data.shader = std::make_shared<NewShader>();
	}
	if (!data.texture) {
		data.texture = std::make_shared<MyTexture>();
	}

	unsigned int indicies[] = { 0, 1, 2, 2, 3, 0 };

	VertexBuffer vb(positions, 4 * 4 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	data.va->SetLayout(layout);
	data.va->SetBuffer(vb);
	data.ib->Init(indicies, 6);

	const char* shader;
	if (shaderType == 0) shader = "Resources/Shaders/Layer.shader";
	else shader = "Resources/Shaders/Cursor.shader";


	data.shader->BindShaderFile(shader);
	data.shader->Bind();

	if (texture != nullptr)
		data.texture->Init(texture);
	else {
		if (vectorOfTexture.size() > 0) {
			data.texture->Init(vectorOfTexture, canvasWidth, canvasHeight);
		} else
			data.texture->Init(canvasWidth, canvasHeight, transparent);
	}

	data.texture->Bind();
	data.shader->SetUniform1i("u_Texture", 0);

	data.va->UnBind();
	data.ib->UnBind();
	data.shader->UnBind();
	data.texture->UnBind();
}

CanvasData NewDraw::initCanvas(unsigned int canvasWidthIn, unsigned int canvasHeightIn, GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float xMult = (float)height / (float)width;

	CanvasData data;
	canvasWidth = canvasWidthIn;
	canvasHeight = canvasHeightIn;
	float canvasRatio;
	float positions[16];

	if (canvasWidth >= canvasHeight) {
		canvasRatio = float(canvasHeight) / float(canvasWidth);
		xScale = 0.8f * xMult;
		yScale = canvasRatio * 0.8f;
	}
	else {
		canvasRatio = float(canvasWidth) / float(canvasHeight);
		xScale = canvasRatio * 0.8f * xMult;
		yScale = 0.8f;	
	}

	fillPositions(positions, xScale, yScale);
	initData(data.data, positions, nullptr);

	glGenFramebuffers(1, &data.data.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, data.data.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.data.texture->GetId(), 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	data.canvasX = xScale;
	data.canvasY = yScale;

	return data;
}


void NewDraw::initLayer(RenderData& data, std::vector<unsigned char> textureData)
{
	float positions[16];
	fillPositions(positions, xScale, yScale);
	initData(data, positions, nullptr, 0, 1, textureData);

	glGenFramebuffers(1, &data.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, data.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.texture->GetId(), 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void NewDraw::InitBrush(RenderData& data, float& radius, std::string texture) 
{
	float positions[16];
	fillPositions(positions, radius, radius);
	if (texture.empty()) {
		initData(data, positions, "Resources/Textures/penBrush.png", 1);
	}
	else {
		initData(data, positions, texture.c_str(), 1);
	}
}

void NewDraw::BrushToPosition(GLFWwindow* window, RenderData& cursor, float& radius, float* aspect, float* offset, float* scale, float* position, int debug) {
	float positions[16];
	float yMult = aspect[0] / aspect[1];
	fillPositions(positions, radius / aspect[0] * scale[0], radius * yMult / aspect[0] * scale[1], (position[0] - offset[0]) / aspect[0], (position[1] - offset[1]) / aspect[1]);
	VertexBuffer vb(positions, 4 * 4 * sizeof(float));
	cursor.va->SetBuffer(vb);
	cursor.va->UnBind();
}

void NewDraw::MoveCanvas(RenderData& canvas, float* size, float* offset) {
	float positions[16];
	fillPositions(positions, size[0], size[1], offset[0], offset[1]);
	VertexBuffer vb(positions, 4 * 4 * sizeof(float));
	canvas.va->SetBuffer(vb);
	canvas.va->UnBind();
}

void FloodFill(int x, int y, ImVec4& targetColor, ImVec4& fillColor, std::vector<unsigned char>& pixels, std::vector<bool>& visited) {
	if (AreColorsEqual(targetColor, fillColor)) return;

	std::stack<std::pair<int, int>> stack;
	stack.push({ x, y });

	const int fillR = static_cast<int>(fillColor.x * 255);
	const int fillG = static_cast<int>(fillColor.y * 255);
	const int fillB = static_cast<int>(fillColor.z * 255);
	const int fillA = static_cast<int>(fillColor.w * 255);

	const int targetR = static_cast<int>(targetColor.x * 255);
	const int targetG = static_cast<int>(targetColor.y * 255);
	const int targetB = static_cast<int>(targetColor.z * 255);
	const int targetA = static_cast<int>(targetColor.w * 255);

	while (!stack.empty()) {
		auto top = stack.top();
		int cx = top.first;
		int cy = top.second;

		stack.pop();

		int index = (cy * canvasWidth + cx) * 4;

		if (visited[index]) continue;

		if (pixels[index] != targetR || pixels[index + 1] != targetG ||
			pixels[index + 2] != targetB || pixels[index + 3] != targetA) {
			continue;
		}

		int left = cx, right = cx;
		while (left > 0) {
			int lIndex = (cy * canvasWidth + (left - 1)) * 4;
			if (pixels[lIndex] != targetR || pixels[lIndex + 1] != targetG ||
				pixels[lIndex + 2] != targetB || pixels[lIndex + 3] != targetA) {
				break;
			}
			left--;
		}
		while (right < canvasWidth - 1) {
			int rIndex = (cy * canvasWidth + (right + 1)) * 4;
			if (pixels[rIndex] != targetR || pixels[rIndex + 1] != targetG ||
				pixels[rIndex + 2] != targetB || pixels[rIndex + 3] != targetA) {
				break;
			}
			right++;
		}

		for (int i = left; i <= right; ++i) {
			int fillIndex = (cy * canvasWidth + i) * 4;

			if (!visited[fillIndex]) {
				pixels[fillIndex] = fillR;
				pixels[fillIndex + 1] = fillG;
				pixels[fillIndex + 2] = fillB;
				pixels[fillIndex + 3] = fillA;
				visited[fillIndex] = true;
			}

			if (cy > 0) stack.push({ i, cy - 1 });
			if (cy < canvasHeight - 1) stack.push({ i, cy + 1 });
		}
	}
}

void NewDraw::Fill(Layer* layer, int xIn, int yIn, ImVec4 fillColor)
{
	std::vector<unsigned char> pixels(canvasWidth * canvasHeight * 4);
	std::vector<bool> visited(canvasWidth * canvasHeight * 4);
	layer->data.texture->Bind();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	int index = (yIn * canvasWidth + xIn) * 4;
	ImVec4 targetColor(
		pixels[index] / 255.0f,
		pixels[index + 1] / 255.0f,
		pixels[index + 2] / 255.0f,
		pixels[index + 3] / 255.0f
	);
	FloodFill(xIn, yIn, targetColor, fillColor, pixels, visited);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
}