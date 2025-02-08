#pragma once

#include "VertexArray.h"
#include "NewShader.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include <iostream>
#include "GLEW/glew.h"
#include "json/json.hpp"

#ifndef ASSERT
#define ASSERT(x) if (!(x)) __debugbreak()
#endif

#ifndef GLCall
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#endif


struct RenderData
{
	std::shared_ptr<VertexArray> va;
	std::shared_ptr<IndexBuffer> ib;
	std::shared_ptr<NewShader> shader;
	std::shared_ptr<MyTexture> texture;
};

struct Position {
	float x, y;

	Position() : x(0), y(0) {}
	Position(float xIn, float yIn) : x(xIn), y(yIn){}
};


void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class NewRenderer {
public:
	bool onUI;

	void Init(GLFWwindow* windowIn, unsigned int& canvasWidthIn, unsigned int& canvasHeightIn, int screenWidth, int screenHeight);
	void Draw(const RenderData& data);
	void Clear();
	void Render();
	void RenderLayers();
	void RenderCursor();
	void RenderCursorToCanvas(int currentLayer = 0);
	void MoveLayers(static float* offset);
	void Zoom(static float scale, static float* cursorPos);
	void OnResize(float& x, float& y, float* offsetIn, float& yRatio);
	void LoadPrevCursor(float* GlCursorPos);
	void SetDrawData();
	void SendDraw();
	void SetColor(float* color);
};
