#pragma once

#include "VertexArray.h"
#include "NewShader.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include <iostream>
#include "GLEW/glew.h"
#include "Messages.h"
#include "json/json.hpp"
#include "ThreadSafeQueue.h"

#include <unordered_map>
#include <string>
#include <memory>

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
	unsigned int fbo = 0;
};

struct Node {
	std::string name;
	bool visible = true;
	bool editing = false;
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
	bool open = false;
	std::vector<int> childrenIds;

	Folder(const std::string& folderName, int idIn)
		: Node(folderName, idIn) {
	}

	void AddChild(int child) {
		childrenIds.push_back(child);
	}
};



void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class NewRenderer {
private:
	std::function<void(const DrawMessage&)> mainThreadCallback;
public:
	bool recieving = false;
	bool onUI;
	int currentNode;
	int nextFreeNodeIndex = 0;
	int currentFolder = 0;
	std::unordered_map<int, std::shared_ptr<Node>> nodes;

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
	void SetColor(float* color);
	void RenderDrawMessage(const DrawMessage& drawMessage);
	void SendDraw();
	void SendLayerRename(std::string nameIn, int locationIn);

	void AddLayer(std::string name, int location);
	void AddFolder(std::string name, int location);

	ThreadSafeQueue taskQueue;


	void ExecuteMainThreadTask(const DrawMessage& drawMessage) {
		taskQueue.push(drawMessage);
	}

	void ProcessTasks() {
		DrawMessage task;
		while (taskQueue.pop(task)) {
			RenderDrawMessage(task);
		}
	}

	void SetMainThreadCallback(std::function<void(const DrawMessage&)> callback) {
		mainThreadCallback = callback;
	}
};
