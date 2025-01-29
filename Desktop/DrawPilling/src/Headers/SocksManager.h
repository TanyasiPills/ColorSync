#pragma once
#include "SocketSource/sio_client.h"
#include <map>
#include <string>

class SManager {
private:
public:
	static void Connect(const char* ip, std::string token, std::map<std::string, std::string> room);
	static void Down();
	static void OnMessage(sio::event& ev);
	static void SendMsg(std::string msg);
};