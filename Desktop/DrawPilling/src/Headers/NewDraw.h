#pragma once
#include "NewRenderer.h"
#include "VertexArray.h"
#include "NewShader.h"
#include "Texture.h"

class NewDraw {
private:

public:
	static CanvasData initCanvas(unsigned int& canvasWidthIn, unsigned int& canvasHeightIn);
	static void initLayer(RenderData& data, float& xScale, float& yScale, std::vector<unsigned char> textureData = {});
	static void InitBrush(RenderData& data, float& radius, std::string texture = "");
	static void BrushToPosition(GLFWwindow* window, RenderData& cursor, float& radius, float* aspect, float* offset, float* scale, float* position, int debug = 0);
	static void MoveCanvas(RenderData& canvas, float* size, float* offset);
	static void Fill(Layer* layer, int xIn, int yIn, ImVec4 fillColor);
};