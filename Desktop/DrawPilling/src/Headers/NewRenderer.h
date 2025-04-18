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
#include "SocialStructs.h"
#include "SocialMedia.h"

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

struct UserPos {
	std::string name;
	Position pos;
	int color;
};

struct RenderData
{
	std::shared_ptr<VertexArray> va;
	std::shared_ptr<IndexBuffer> ib;
	std::shared_ptr<NewShader> shader;
	std::shared_ptr<MyTexture> texture;
	unsigned int fbo = 0;
};

struct CanvasData
{
	RenderData data;
	float canvasX;
	float canvasY;
};

struct RoomUser {
	int id;
	std::string username;
	bool admin = false;

	RoomUser() = default;
	RoomUser(const int idIn, const std::string nameIn, bool isOwner) : id(idIn), username(nameIn), admin(isOwner){}
};

struct Node {
	std::string name;
	bool visible = true;
	bool editing = false;
	bool selected = false;
	int id;
	int opacity = 100;

	Node() = default;

	Node(const std::string& nodeName, int idIn) : name(nodeName), id(idIn) {}
	virtual ~Node() = default;
};

struct Layer : public Node {
	RenderData data;

	Layer() = default;

	Layer(const std::string& layerName, int idIn, RenderData dataIn)
		: Node(layerName, idIn), data(dataIn) {
	}
};

struct Folder : public Node {
	bool open = false;
	std::vector<int> childrenIds;

	Folder() = default;

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
	bool isEditor = false;
	int tool = 0;
	int currentLayerToDrawOn = 0;

	std::vector<RenderData> brushes;
	std::unordered_map<int, UserMoveMessage> usersToMove;
	std::unordered_map<int, std::shared_ptr<Node>> nodes;
	std::vector<int> layers;
	std::vector<int> folders;

	bool inited = false;

	void Init(GLFWwindow* windowIn);
	void InitBrushes();
	void InitNewCanvas();
	void SetDrawData(unsigned int& canvasWidthIn, unsigned int& canvasHeightIn);

	void Draw(const RenderData& data);
	void Clear();
	void Render();
	void RenderLayers();
	void RenderCursor();
	void RenderCursorToCanvas();
	void MoveLayers(static float* offset);
	void Zoom(static float scale, static float* cursorPos);
	void OnResize(float& x, float& y, float* offsetIn, float& yRatio);
	void LoadPrevCursor(double* GlCursorPos);
	void SetDrawDataJa();
	void SetColor(float* color);
	void RenderDrawMessage(const DrawMessage& drawMessage);
	void SendDraw();
	void SendLayerRename(std::string nameIn, int locationIn);

	int GetParent(int& id);
	int CreateLayer(int& parent);
	int CreateFolder(int& parent);
	void RemoveLayer(int& index);
	void RemoveFolder(int& index);

	void ChangeBrush(int index);

	void SetOnline(bool value);
	bool GetOnline();

	unsigned int* GetCanvasSize();
	void SetCanvasSize(unsigned int* sizes);

	ThreadSafeQueue taskQueue;


	void ExecuteMainThreadTask(const DrawMessage& drawMessage) {
		taskQueue.push(drawMessage);
	}

	void ProcessTasks() {
		DrawMessage task;
		while (taskQueue.pop(task)) {
			if (task.location != -1) {
				CreateLayer(task.location);
			}
			else {
				RenderDrawMessage(task);
			}
		}
	}

	void SetMainThreadCallback(std::function<void(const DrawMessage&)> callback) {
		mainThreadCallback = callback;
	}

	void SwapView(bool isOnline);
};
