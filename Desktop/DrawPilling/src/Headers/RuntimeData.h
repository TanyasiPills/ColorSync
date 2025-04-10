#pragma once
#include <string>
#include <unordered_set>
#include "GLEW/glew.h"

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
    std::string email = "";

    bool logedIn = true;
    int id = -1;

    GLuint pfpTexture = 0;

    int undoCount = 30;

    std::unordered_set<int> liked;
};