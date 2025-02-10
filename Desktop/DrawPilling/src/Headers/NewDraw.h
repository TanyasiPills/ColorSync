#pragma once
#include "NewRenderer.h"
#include "VertexArray.h"
#include "NewShader.h"
#include "Texture.h"

struct CanvasData
{
	RenderData data;
	float canvasX;
	float canvasY;
};

class NewDraw {
private:

public:
	static CanvasData initCanvas(unsigned int& canvasWidthIn, unsigned int& canvasHeightIn);
	static void initLayer(RenderData& data, float& xScale, float& yScale);
	static void InitBrush(RenderData& data, float& radius);
	static void BrushToPosition(GLFWwindow* window, RenderData& cursor, float& radius, float* aspect, float* offset, float* scale, float* position, int debug = 0);
	static void MoveCanvas(RenderData& canvas, float* size, float* offset);
};