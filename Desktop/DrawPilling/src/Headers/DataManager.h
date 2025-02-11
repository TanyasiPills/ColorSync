#pragma once
#include <cstring>
#include <iostream>
#include <fstream>

struct ApplicationData {
    char token[256];
    char name[25];
};

class DataManager
{
private:
public:
    static void SaveData(const ApplicationData& data, const std::string& filename);
    static ApplicationData LoadData(const std::string& filename);
    static void LoadAppData();
    static void SaveAppData(std::string nameIn, std::string tokenIn);
};