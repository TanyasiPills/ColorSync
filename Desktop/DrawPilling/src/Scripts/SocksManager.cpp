#include "SocksManager.h"
#include "DrawUI.h"
#include <iostream>

sio::client h;
const char* ip;

void SManager::Connect(const char* ip, std::string token, std::map<std::string, std::string> room)
{
    h.set_close_listener([](const sio::client::close_reason& reason) {
        std::cout << "Connection closed nya! " << std::endl;
        });
    h.socket()->on("error", [](sio::event& ev) {
        std::cout << "Connection failed nya!" << std::endl;
        });
    h.set_fail_listener([]() {
        std::cout << "The nyaggers crashed the server >:c" << std::endl;
        });

    h.socket()->on("message", [](sio::event& ev) {
        SManager::OnMessage(ev);
        });

    h.socket()->on("message", OnMessage);
    h.socket()->on("action", OnAction);

    std::map<std::string, std::string> headers;
    headers["token"] = token;

    h.connect(ip, room, headers);
}

void SManager::Down()
{
    h.socket()->close();
}

void SManager::OnAction(sio::event& ev) {

}

void SManager::SendAction(int type, std::map<std::string, std::string> dataIn)
{
    sio::message::ptr msg = sio::object_message::create();
    sio::message::ptr data = sio::object_message::create();

    switch (type)
    {
        case Draw:
            msg->get_map()["type"] = sio::string_message::create("draw");
            //data->get_map()[""] = 
        default:
            break;
    }
    msg->get_map()["data"] = data;

    h.socket()->emit("action", msg);
}

void SManager::OnMessage(sio::event& ev)
{
    sio::object_message::ptr nem = ev.get_message();
    std::string username = nem->get_map()["username"]->get_string();
    std::string message = nem->get_map()["message"]->get_string();
    std::map<std::string, std::string> messageObject;
    messageObject["username"] = username;
    messageObject["message"] = message;
    DrawUI::GetMsg(messageObject);

}

void SManager::SendMsg(std::string msg) {
    sio::message::ptr json_msg = sio::object_message::create();
    json_msg->get_map()["message"] = sio::string_message::create(msg);
    h.socket()->emit("message", json_msg);
}
