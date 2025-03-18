#pragma once
#include "SocketSource/sio_client.h"
#include <map>
#include <string>
#include "json/json.hpp"
#include <vector>
#include "NewRenderer.h"
#include "Messages.h"
#include "GLEW/glew.h"
#include <GLFW/glfw3.h> 

enum Type {
	Draw = 0,
	AddNode = 1,
	RenameNode = 2,
	DeleteNode = 3,
	PlayerLeave = 4,
	PlayerJoin = 5,
};

class SManager {
private:
public:
	static void SetRenderer(NewRenderer& renderer);
	static void Connect(std::string ip, std::map<std::string, std::string> header, std::map<std::string, std::string> room);
	static void Down();

	static void OnMessage(sio::event& ev);
	static void SendMsg(std::string msg);

	static void OnAction(sio::event& ev);
	static void SendAction(Message& data);

	static void OnSystemMessage(sio::event& ev);

	static void ProcessHistory();
	static unsigned int* GetCanvasSize();
	static void SetCanvasSize(unsigned int width, unsigned int height);
};