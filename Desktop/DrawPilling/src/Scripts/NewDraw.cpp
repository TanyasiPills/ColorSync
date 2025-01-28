#include "NewDraw.h"
#include "CallBacks.h"
#include <iostream>
#include <algorithm>

unsigned int canvasWidth = 0, canvasHeight = 0;

void fillPositions(float* positions, float xScale = 0.0, float yScale = 0.0, float xPos = 0.0, float yPos = 0.0) {
	positions[0] = (-xScale) + xPos; positions[1] = (-yScale) + yPos; positions[2] = 0.0f; positions[3] = 0.0f;
	positions[4] = xScale + xPos; positions[5] = (-yScale) + yPos; positions[6] = 1.0f; positions[7] = 0.0f;
	positions[8] = xScale + xPos; positions[9] = yScale + yPos; positions[10] = 1.0f; positions[11] = 1.0f;
	positions[12] = (-xScale) + xPos; positions[13] = yScale + yPos; positions[14] = 0.0f; positions[15] = 1.0f;
}

void initData(RenderData& data, float* positions, const char* texture = nullptr, int shaderType = 0)
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
	else
		data.texture->Init(canvasWidth, canvasHeight);

	data.texture->Bind();
	data.shader->SetUniform1i("u_Texture", 0);

	data.va->UnBind();
	data.ib->UnBind();
	data.shader->UnBind();
	data.texture->UnBind();
}

CanvasData NewDraw::initCanvas(unsigned int& canvasWidthIn, unsigned int& canvasHeightIn)
{
	CanvasData data;
	canvasWidth = canvasWidthIn;
	canvasHeight = canvasHeightIn;
	float canvasRatio;
	float positions[16];
	float xScale = 1;
	float yScale = 1;

	if (canvasWidthIn >= canvasHeightIn) {
		canvasRatio = float(canvasHeightIn) / float(canvasWidthIn);
		xScale = 0.8f;
		yScale = canvasRatio * 0.8f;
	}
	else {
		canvasRatio = float(canvasWidthIn) / float(canvasHeightIn);
		xScale = canvasRatio * 0.8f;
		yScale = 0.8f;	
	}
	fillPositions(positions, xScale, yScale);
	initData(data.data, positions, nullptr);
	data.canvasX = xScale;
	data.canvasY = yScale;

	return data;
}


void NewDraw::initLayer(RenderData& data, float& xScale, float& yScale)
{
	float positions[16];
	fillPositions(positions, xScale, yScale);
	initData(data, positions, nullptr);
}

void NewDraw::InitBrush(RenderData& data, float& radius) 
{
	float positions[16];
	fillPositions(positions, radius, radius);
	initData(data, positions, "Resources/Textures/circle.png", 1);
}

void NewDraw::BrushToPosition(GLFWwindow* window, RenderData& cursor, float& radius, float*aspect, float* offset, float* scale, float* position) {
	float positions[16];
	float yMult = aspect[0] / aspect[1];
	fillPositions(positions, radius/aspect[0] * scale[0], radius * yMult / aspect[0] * scale[1], (position[0] - offset[0]) / aspect[0], (position[1] - offset[1]) / aspect[1]);
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