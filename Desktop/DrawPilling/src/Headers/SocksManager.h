#pragma once
#include "SocketSource/sio_client.h"
#include <map>
#include <string>
#include "json/json.hpp"
#include <vector>
#include "NewRenderer.h"

enum Type {
	Draw = 0,
	AddNode = 1,
	RenameNode = 2,
	Undo = 3,
	Redo = 4,
	Delete = 5,
	Move = 6
};

class SManager {
private:
public:
	static void SetRenderer(NewRenderer& renderer);
	static void Connect(const char* ip, std::string token, std::map<std::string, std::string> room);
	static void Down();

	static void OnMessage(sio::event& ev);
	static void SendMsg(std::string msg);

	static void OnAction(sio::event& ev);
	static void SendAction(Message& data);

};