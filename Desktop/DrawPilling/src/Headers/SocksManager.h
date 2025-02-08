#pragma once
#include "SocketSource/sio_client.h"
#include <map>
#include <string>
#include "json/json.hpp"

enum Type {
	None = 0,
	Draw = 1 << 0,
	AddLayer = 1 << 1,
	RenameLayer = 1 << 2,
	Undo = 1 << 3,
	Redo = 1 << 4,
};

class SManager {
private:
public:
	static void Connect(const char* ip, std::string token, std::map<std::string, std::string> room);
	static void Down();

	static void OnMessage(sio::event& ev);
	static void SendMsg(std::string msg);

	static void OnAction(sio::event& ev);
	static void SendAction(int type, std::map<std::string, std::string> data);

};