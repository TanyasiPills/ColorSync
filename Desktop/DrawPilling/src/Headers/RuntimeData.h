#pragma once
#include <string>

class RuntimeData
{
private:
    RuntimeData() = default;
public:
    static RuntimeData& getInstance()
    {
        static RuntimeData instance;
        return instance;
    }

    std::string ip = "";
    std::string token = "";

    std::string username = "";
    std::string password = "";

    int undoCount = 30;
};