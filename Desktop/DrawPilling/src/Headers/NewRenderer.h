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

struct Node {
	std::string name;
	bool visible = true;
	bool editing = false;
	bool open = false;
	int id;

	Node(const std::string& nodeName, int idIn) : name(nodeName), id(idIn) {}
	virtual ~Node() = default;
};

struct Layer : public Node {
	RenderData data;

	Layer(const std::string& layerName, int idIn, RenderData dataIn)
		: Node(layerName, idIn), data(dataIn) {
	}
};

struct Folder : public Node {
	std::vector<std::unique_ptr<Node>> children;

	Folder(const std::string& folderName, int idIn)
		: Node(folderName, idIn) {
	}

	void AddChild(std::unique_ptr<Node> child) {
		children.push_back(std::move(child));
	}
};

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class NewRenderer {
public:
	bool onUI;
	int currentLayer;
	int lastLayerIndex = 0;

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
