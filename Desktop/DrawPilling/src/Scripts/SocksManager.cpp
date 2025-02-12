#include "SocksManager.h"
#include "DrawUI.h"
#include <iostream>
#include <GLFW/glfw3.h>


sio::client h;
const char* ip;
bool onserver = false;
NewRenderer* rendererSocks;

void SManager::SetRenderer(NewRenderer& rendererIn)
{
    rendererSocks = &rendererIn;
}

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
    switch (type)
    {
        case Draw:
            try {
                DrawMessage drawMessage;
                drawMessage.layer = data["layer"]->get_int();
                drawMessage.brush = data["brush"]->get_int();
                drawMessage.size = data["size"]->get_double();

                std::vector<sio::message::ptr> positionArray = data["positions"]->get_vector();
                for (int i = 0; i < positionArray.size(); ++i) {
                    std::map<std::string, sio::message::ptr> position = positionArray[i]->get_map();
                    drawMessage.positions.push_back(Position(position["x"]->get_double(), position["y"]->get_double()));
                }

                std::map<std::string, sio::message::ptr> colors = data["color"]->get_map();
                std::cout << colors["r"]->get_double() << std::endl;
                drawMessage.color[0] = colors["r"]->get_double();
                drawMessage.color[1] = colors["g"]->get_double();
                drawMessage.color[2] = colors["b"]->get_double();

                std::map<std::string, sio::message::ptr> offsets = data["offset"]->get_map();
                drawMessage.offset.x = offsets["x"]->get_double();
                drawMessage.offset.y = offsets["y"]->get_double();

                std::map<std::string, sio::message::ptr> ratio = data["ratio"]->get_map();
                drawMessage.ratio.x = ratio["x"]->get_double();
                drawMessage.ratio.y = ratio["y"]->get_double();

                std::map<std::string, sio::message::ptr> curSca = data["CurSca"]->get_map();
                drawMessage.cursorScale[0] = curSca["x"]->get_double();
                drawMessage.cursorScale[1] = curSca["y"]->get_double();
                drawMessage.cursorScale[2] = curSca["z"]->get_double();
                
                rendererSocks->ExecuteMainThreadTask(drawMessage);
                //rendererSocks->RenderDrawMessage(drawMessage);
            }
            catch(...){
                std::cerr << "Error recieving DrawMessage" << std::endl;
            }
            break;
        case AddNode:
            try {
                int typeOfNode = data["node"]->get_int();
                if (typeOfNode == 0) {
                    rendererSocks->AddLayer(data["name"]->get_string(), data["location"]->get_int());
                }
                else if (typeOfNode == 1) {
                    rendererSocks->AddFolder(data["name"]->get_string(), data["location"]->get_int());
                }
            }
            catch (...) {
                std::cerr << "Error recieving AddNodeMessage" << std::endl;
            }
            break;
        case RenameNode:
            try {
                Node* node = dynamic_cast<Node*>(rendererSocks->nodes[data["location"]->get_int()].get());
                node->name = data["name"]->get_string();
            }
            catch (...) {
                std::cerr << "Error recieving RenameNodeMessage" << std::endl;
            }
            break;
        default:
            break;
    }
}

void SManager::SendAction(Message& dataIn)
{
    if (!onserver) return;
    sio::message::ptr msg = sio::object_message::create();
    sio::message::ptr data = sio::object_message::create();


    switch (dataIn.type)
    {
        case Draw:
            try {
                DrawMessage* draw = dynamic_cast<DrawMessage*>(&dataIn);
                msg->get_map()["type"] = sio::int_message::create(Draw);
                data->get_map()["layer"] = sio::int_message::create(draw->layer);
                data->get_map()["brush"] = sio::int_message::create(draw->brush);
                data->get_map()["size"] = sio::double_message::create(draw->size);
                try {
                    sio::message::ptr positionsArray = sio::array_message::create();
                    for (int i = 0; i < draw->positions.size(); i++)
                    {
                        sio::message::ptr posObj = sio::object_message::create();
                        posObj->get_map()["x"] = sio::double_message::create(draw->positions[i].x);
                        posObj->get_map()["y"] = sio::double_message::create(draw->positions[i].y);
                        positionsArray->get_vector().push_back(posObj);
                    }
                    data->get_map()["positions"] = positionsArray;

                }
                catch (...) {
                    std::cerr << "Error parsing positions JSON" << std::endl;
                }
                try {
                    sio::message::ptr offsetObj = sio::object_message::create();

                    offsetObj->get_map()["x"] = sio::double_message::create(draw->offset.x);
                    offsetObj->get_map()["y"] = sio::double_message::create(draw->offset.y);

                    data->get_map()["offset"] = offsetObj;
                }
                catch (...) {
                    std::cerr << "Error parsing offset JSON" << std::endl;
                }
                try {
                    sio::message::ptr offsetObj = sio::object_message::create();

                    offsetObj->get_map()["x"] = sio::double_message::create(draw->ratio.x);
                    offsetObj->get_map()["y"] = sio::double_message::create(draw->ratio.y);

                    data->get_map()["ratio"] = offsetObj;
                }
                catch (...) {
                    std::cerr << "Error parsing ratio JSON" << std::endl;
                }
                try {
                    sio::message::ptr colorObj = sio::object_message::create();

                    colorObj->get_map()["r"] = sio::double_message::create(draw->color[0]);
                    colorObj->get_map()["g"] = sio::double_message::create(draw->color[1]);
                    colorObj->get_map()["b"] = sio::double_message::create(draw->color[2]);

                    data->get_map()["color"] = colorObj;
                }
                catch (...) {
                    std::cerr << "Error parsing color JSON" << std::endl;
                }
                try {
                    sio::message::ptr cursorScaleObj = sio::object_message::create();

                    cursorScaleObj->get_map()["x"] = sio::double_message::create(draw->cursorScale[0]);
                    cursorScaleObj->get_map()["y"] = sio::double_message::create(draw->cursorScale[1]);
                    cursorScaleObj->get_map()["z"] = sio::double_message::create(draw->cursorScale[2]);

                    data->get_map()["CurSca"] = cursorScaleObj;
                }
                catch (...) {
                    std::cerr << "Error parsing cursorscale JSON" << std::endl;
                }
            }
            catch (...) {
                std::cerr << "bad data type: not DrawMessage" << std::endl;
            }
            break;
        case AddNode:
            try {
                NodeAddMessage* node = dynamic_cast<NodeAddMessage*>(&dataIn);
                msg->get_map()["type"] = sio::int_message::create(AddNode);
                data->get_map()["node"] = sio::int_message::create(node->nodeType);
                data->get_map()["name"] = sio::string_message::create(node->name);
                data->get_map()["location"] = sio::int_message::create(node->location);
            }
            catch (...) {
                std::cerr << "bad data type: not NodeMessage" << std::endl;
            }
            break;
        case RenameNode:
            try {
                NodeRenameMessage* node = dynamic_cast<NodeRenameMessage*>(&dataIn);
                msg->get_map()["type"] = sio::int_message::create(RenameNode);
                data->get_map()["name"] = sio::string_message::create(node->name);
                data->get_map()["location"] = sio::int_message::create(node->location);
            }
            catch (...) {
                std::cerr << "bad data type: not NodeMessage" << std::endl;
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