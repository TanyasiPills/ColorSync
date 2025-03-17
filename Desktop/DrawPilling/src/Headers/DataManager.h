#pragma once
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include "NewRenderer.h"

struct Sync {
    unsigned int canvasWidth;
    unsigned int canvasHeight;
    int layerLength;
    std::vector<Layer> layers;
    std::vector<std::vector<unsigned char>> layerTextures;
    int folderLength;
    std::vector<Folder> folders;
};

struct ApplicationData {
    char token[256];
    char name[25];
    char ip[100];
};

class DataManager
{
private:
public:
    static void SetRenderer(NewRenderer& rendererIn);

    static void LoadAppData();
    static void SaveAppData();

    static void SaveSyncData(std::string path);
    static void LoadSyncData(std::string path);
};