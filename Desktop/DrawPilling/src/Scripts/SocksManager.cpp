#include "SocksManager.h"
#include "DrawUI.h"
#include <iostream>

sio::client h;
const char* ip;
bool onserver = false;

void SManager::Connect(const char* ip, std::string token, std::map<std::string, std::string> room)
{
    h.set_close_listener([](const sio::client::close_reason& reason) {
        std::cout << "Connection closed nya! " << std::endl;
        onserver = false;
        });
    h.socket()->on("error", [](sio::event& ev) {
        std::cout << "Error: ";
        sio::object_message::ptr nem = ev.get_message();
        std::string message = nem->get_map()["message"]->get_string();
        std::cout << message << std::endl;
        });
    h.set_fail_listener([]() {
        std::cout << "The nyaggers crashed the server >:c" << std::endl;
        onserver = false;
        });

    h.socket()->on("message", [](sio::event& ev) {
        SManager::OnMessage(ev);
        });

    h.set_open_listener([]() {
        std::cout << "The nyaggers on the server :>" << std::endl;
        onserver = true;
        
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
    sio::object_message::ptr dataIn = ev.get_message();
    int type = dataIn->get_map()["type"]->get_int();
    std::map<std::string, sio::message::ptr> data = dataIn->get_map()["data"]->get_map();
    DrawMessage drawMessage;
    switch (type)
    {
        case Draw:
            try {
                drawMessage.layer = data["layer"]->get_int();
                drawMessage.brush = data["brush"]->get_int();

                std::vector<sio::message::ptr> positionArray = data["positions"]->get_vector();
                for (int i = 0; i < positionArray.size(); ++i) {
                    std::map<std::string, sio::message::ptr> position = positionArray[i]->get_map();
                    drawMessage.positions.push_back(Position(position["x"]->get_double(), position["y"]->get_double()));
                }

                std::map<std::string, sio::message::ptr> colors = data["color"]->get_map();
                drawMessage.color[0] = colors["r"]->get_double();
                drawMessage.color[1] = colors["g"]->get_double();
                drawMessage.color[2] = colors["b"]->get_double();

                std::map<std::string, sio::message::ptr> offsets = data["offset"]->get_map();
                drawMessage.offset.x = offsets["x"]->get_double();
                drawMessage.offset.y = offsets["y"]->get_double();
            }
            catch(...){
                std::cerr << "hihi" << std::endl;
            }
            break;
        default:
            break;
    }
}

void SManager::SendAction(int type, DrawMessage dataIn)
{
    if (!onserver) return;
    sio::message::ptr msg = sio::object_message::create();
    sio::message::ptr data = sio::object_message::create();


    switch (type)
    {
        case Draw:
            msg->get_map()["type"] = sio::int_message::create(Draw);
            data->get_map()["layer"] = sio::int_message::create(dataIn.layer);
            data->get_map()["brush"] = sio::int_message::create(dataIn.brush);
            data->get_map()["size"] = sio::double_message::create(dataIn.size);

            try {
                sio::message::ptr positionsArray = sio::array_message::create();
                for (int i = 0; i < dataIn.positions.size(); i++)
                {
                    sio::message::ptr posObj = sio::object_message::create();
                    posObj->get_map()["x"] = sio::double_message::create(dataIn.positions[i].x);
                    posObj->get_map()["y"] = sio::double_message::create(dataIn.positions[i].y);
                    positionsArray->get_vector().push_back(posObj);
                }
                data->get_map()["positions"] = positionsArray;

            }
            catch (...) {
                std::cerr << "Error parsing positions JSON" << std::endl;
            }
            try {
                sio::message::ptr offsetObj = sio::object_message::create();

                offsetObj->get_map()["x"] = sio::double_message::create(dataIn.offset.x);
                offsetObj->get_map()["y"] = sio::double_message::create(dataIn.offset.y);

                data->get_map()["offset"] = offsetObj;
            }
            catch (...) {
                std::cerr << "Error parsing offset JSON" << std::endl;
            }
            try {
                sio::message::ptr colorObj = sio::object_message::create();

                colorObj->get_map()["r"] = sio::double_message::create(dataIn.color[0]);
                colorObj->get_map()["g"] = sio::double_message::create(dataIn.color[1]);
                colorObj->get_map()["b"] = sio::double_message::create(dataIn.color[2]);

                data->get_map()["color"] = colorObj;
            }
            catch (...) {
                std::cerr << "Error parsing color JSON" << std::endl;
            }
            break;
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