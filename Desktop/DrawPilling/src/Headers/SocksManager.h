#pragma once
#include "SocketSource/sio_client.h"
#include <map>
#include <string>
#include "json/json.hpp"
#include <vector>
#include "NewRenderer.h"

enum Type {
	Draw = 0,
	AddLayer = 1,
	RenameLayer = 2,
	Undo = 3,
	Redo = 4,
};

struct DrawMessage {
	int layer;
	int brush;
	float size;
	std::vector<Position> positions;
	Position offset;
	float* color;

	DrawMessage(): layer(0), brush(0), size(0.0f), positions(), offset(), color(nullptr) {}
};

class SManager {
private:
public:
	static void Connect(const char* ip, std::string token, std::map<std::string, std::string> room);
	static void Down();

	static void OnMessage(sio::event& ev);
	static void SendMsg(std::string msg);

	static void OnAction(sio::event& ev);
	static void SendAction(int type, DrawMessage data);

};