#include "DrawUI.h"
#include "DataManager.h"
#include <filesystem>
#include "RuntimeData.h"


ApplicationData appdata;

Sync drawdata;

static NewRenderer* renderer;

void DataManager::SetRenderer(NewRenderer* rendererIn)
{
    renderer = rendererIn;
}


void SetStringValue(char* dest, const char* value, size_t size) {
    std::strncpy(dest, value, size - 1);
    dest[size - 1] = '\0';
}

void SetAppDataData(ApplicationData& data, std::string nameIn, std::string tokenIn, std::string ipIn) {
    SetStringValue(data.name, nameIn.c_str(), sizeof(data.name));
    SetStringValue(data.token, tokenIn.c_str(), sizeof(data.token));
    SetStringValue(data.ip, ipIn.c_str(), sizeof(data.ip));
}

void SaveAppDataData(const ApplicationData& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file) file.write(reinterpret_cast<const char*>(&data), sizeof(data));
}

ApplicationData LoadAppDataData(const std::string& filename) {
    ApplicationData data;
    std::ifstream file(filename, std::ios::binary);
    if (file) file.read(reinterpret_cast<char*>(&data), sizeof(data));
    else {
        data.name[0] = '\0';
        data.token[0] = '\0';
        data.ip[0] = '\0';
    }
    return data;
}

void DataManager::LoadAppData()
{
    appdata = LoadAppDataData("appdata.bin");

    auto& runtime = RuntimeData::getInstance();
    runtime.ip = appdata.ip;
    runtime.username = appdata.name;
    runtime.token = appdata.token;
}

void DataManager::SaveAppData()
{   
    auto& runtime = RuntimeData::getInstance();
    SetAppDataData(appdata, runtime.username, runtime.token, runtime.ip);
    SaveAppDataData(appdata, "appdata.bin");
}


void SetSyncData()
{
    unsigned int* sizes = renderer->GetCanvasSize();
    drawdata.canvasWidth = sizes[0];
    drawdata.canvasWidth = sizes[1];

    drawdata.layerLength = renderer->layers.size();
    for (int item : renderer->layers)
    {
        Layer* toAdd = dynamic_cast<Layer*>(renderer->nodes[item].get());
        drawdata.layers.push_back(*toAdd);
    }

    drawdata.folderLength = renderer->folders.size();
    for (int item : renderer->folders)
    {
        Folder* toAdd = dynamic_cast<Folder*>(renderer->nodes[item].get());
        drawdata.folders.push_back(*toAdd);
    }
}

void DataManager::SaveSyncData(std::string path)
{
    SetSyncData();
}

void DataManager::LoadSyncData()
{

}