#pragma once
#include "NewRenderer.h"
#include "VertexArray.h"
#include "NewShader.h"
#include "Texture.h"

class NewDraw {
private:

public:
	static CanvasData initCanvas(unsigned int canvasWidthIn, unsigned int canvasHeightIn, GLFWwindow* window);
	static void initLayer(RenderData& data, std::vector<unsigned char> textureData = {});
	static void InitBrush(RenderData& data, float& radius, std::string texture = "");
	static void BrushToPosition(GLFWwindow* window, RenderData& cursor, float& radius, float* aspect, float* offset, float* scale, double* position, int debug = 0);
	static void MoveCanvas(RenderData& canvas, float* size, float* offset);
	static void Fill(Layer* layer, int xIn, int yIn, ImVec4 fillColor);
};