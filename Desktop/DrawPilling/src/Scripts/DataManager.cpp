#include "DrawUI.h"
#include "DataManager.h"
#include <filesystem>
#include "RuntimeData.h"


ApplicationData appdata;




void SetValue(char* dest, const char* value, size_t size) {
    std::strncpy(dest, value, size - 1);
    dest[size - 1] = '\0';
}

void SetData(ApplicationData& data, std::string nameIn, std::string tokenIn, std::string ipIn) {
    SetValue(data.name, nameIn.c_str(), sizeof(data.name));
    SetValue(data.token, tokenIn.c_str(), sizeof(data.token));
    SetValue(data.ip, ipIn.c_str(), sizeof(data.ip));
}

void DataManager::SaveData(const ApplicationData& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file) file.write(reinterpret_cast<const char*>(&data), sizeof(data));
}

ApplicationData DataManager::LoadData(const std::string& filename) {
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
    appdata = DataManager::LoadData("appdata.bin");

    auto& runtime = RuntimeData::getInstance();
    runtime.ip = appdata.ip;
    runtime.username = appdata.name;
    runtime.token = appdata.token;
}
void DataManager::SaveAppData()
{   
    auto& runtime = RuntimeData::getInstance();
    SetData(appdata, runtime.username, runtime.token, runtime.ip);
    DataManager::SaveData(appdata, "appdata.bin");
}

void DataManager::SaveSync(std::string path)
{
    DataManager::SaveData()
}