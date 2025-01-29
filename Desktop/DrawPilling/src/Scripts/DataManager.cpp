#include "DrawUI.h"
#include "DataManager.h"
#include <filesystem>


ApplicationData appdata;


void SetValue(char* dest, const char* value, size_t size) {
    std::strncpy(dest, value, size - 1);
    dest[size - 1] = '\0';
}

void SetData(ApplicationData& data) {
    SetValue(data.name, "nyagger", sizeof(data.name));
}

void DataManager::SaveData(const ApplicationData& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file) file.write(reinterpret_cast<const char*>(&data), sizeof(data));
}

ApplicationData DataManager::LoadData(const std::string& filename) {
    ApplicationData data;
    std::ifstream file(filename, std::ios::binary);
    if (file) file.read(reinterpret_cast<char*>(&data), sizeof(data));
    return data;
}

void DataManager::LoadAppData()
{
    appdata = DataManager::LoadData("appdata.bin");
    DrawUI::InitData(appdata.name, appdata.token);
}
void DataManager::SaveAppData()
{
    SetData(appdata);
    DataManager::SaveData(appdata, "appdata.bin");
}