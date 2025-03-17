#include "DrawUI.h"
#include "DataManager.h"
#include <filesystem>
#include "RuntimeData.h"
#include "NewDraw.h"
#include <windows.h>


ApplicationData appdata;

Sync sync;

static NewRenderer* renderer;
std::vector<GLuint> textures;

void DataManager::SetRenderer(NewRenderer& rendererIn)
{
    renderer = &rendererIn;
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
    for (GLuint& texture : textures) glDeleteTextures(1, &texture);
    textures.clear();

    sync.layers.clear();
    sync.folders.clear();
    sync.layerTextures.clear();

    unsigned int* sizes = renderer->GetCanvasSize();
    std::cout << "syncwidth: " << sizes[0] << ", syncheight: " << sizes[1] << std::endl;
    sync.canvasWidth = sizes[0];
    sync.canvasHeight = sizes[1];

    sync.layerLength = renderer->layers.size();
    for (int item : renderer->layers)
    {
        Layer* toAdd = dynamic_cast<Layer*>(renderer->nodes[item].get());
        sync.layers.push_back(*toAdd);

        GLuint id = toAdd->data.texture->GetId();

        glBindTexture(GL_TEXTURE_2D, id);
        std::vector<unsigned char> pixels(sync.canvasWidth * sync.canvasHeight * 4);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glBindTexture(GL_TEXTURE_2D, 0);
        sync.layerTextures.push_back(pixels);
    }

    sync.folderLength = renderer->folders.size();
    for (int item : renderer->folders)
    {
        Folder* toAdd = dynamic_cast<Folder*>(renderer->nodes[item].get());
        sync.folders.push_back(*toAdd);
    }
}

void CheckAndDeleteFile(const std::string& filePath) {

    DWORD fileAttrib = GetFileAttributes(filePath.c_str());

    if (fileAttrib != INVALID_FILE_ATTRIBUTES && !(fileAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        if (DeleteFile(filePath.c_str())) {
            std::cout << "File deleted successfully!" << std::endl;
        }
        else {
            std::cerr << "Failed to delete file!" << std::endl;
        }
    }
    else {
        std::cout << "File does not exist or it's a directory." << std::endl;
    }
}

void SaveSync(std::string path)
{
    std::ofstream syncFile(path, std::ios::binary);
    if (!syncFile) return;

    syncFile.write(reinterpret_cast<const char*>(&sync.canvasWidth), sizeof(sync.canvasWidth));
    syncFile.write(reinterpret_cast<const char*>(&sync.canvasHeight), sizeof(sync.canvasHeight));

    syncFile.write(reinterpret_cast<const char*>(&sync.layerLength), sizeof(sync.layerLength));


    for (const auto& layer : sync.layers) {
        int nameLength = layer.name.size();
        syncFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        syncFile.write(layer.name.c_str(), nameLength);

        syncFile.write(reinterpret_cast<const char*>(&layer.visible), sizeof(layer.visible));
        syncFile.write(reinterpret_cast<const char*>(&layer.editing), sizeof(layer.editing));
        syncFile.write(reinterpret_cast<const char*>(&layer.selected), sizeof(layer.selected));
        syncFile.write(reinterpret_cast<const char*>(&layer.id), sizeof(layer.id));
        syncFile.write(reinterpret_cast<const char*>(&layer.opacity), sizeof(layer.opacity));
    }
    for (auto& pixels : sync.layerTextures)
    {
        syncFile.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
    }

    syncFile.write(reinterpret_cast<const char*>(&sync.folderLength), sizeof(sync.folderLength));

    for (const auto& folder : sync.folders) {
        int nameLength = folder.name.size();
        syncFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        syncFile.write(folder.name.c_str(), nameLength);

        syncFile.write(reinterpret_cast<const char*>(&folder.visible), sizeof(folder.visible));
        syncFile.write(reinterpret_cast<const char*>(&folder.editing), sizeof(folder.editing));
        syncFile.write(reinterpret_cast<const char*>(&folder.selected), sizeof(folder.selected));
        syncFile.write(reinterpret_cast<const char*>(&folder.id), sizeof(folder.id));
        syncFile.write(reinterpret_cast<const char*>(&folder.opacity), sizeof(folder.opacity));

        syncFile.write(reinterpret_cast<const char*>(&folder.open), sizeof(folder.open));

        int childrenSize = folder.childrenIds.size();
        syncFile.write(reinterpret_cast<const char*>(&childrenSize), sizeof(childrenSize));
        syncFile.write(reinterpret_cast<const char*>(folder.childrenIds.data()), sizeof(int) * childrenSize);
    }

    syncFile.close();
}

void LoadSync(std::string path)
{
    std::ifstream syncFile(path, std::ios::binary);
    if (!syncFile) return;

    syncFile.read(reinterpret_cast<char*>(&sync.canvasWidth), sizeof(sync.canvasWidth));
    syncFile.read(reinterpret_cast<char*>(&sync.canvasHeight), sizeof(sync.canvasHeight));

    int layerSize;
    syncFile.read(reinterpret_cast<char*>(&layerSize), sizeof(layerSize));
    sync.layers.resize(layerSize);

    for (auto& layer : sync.layers) {
        int nameLength;
        syncFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        layer.name.resize(nameLength);
        syncFile.read(&layer.name[0], nameLength);

        syncFile.read(reinterpret_cast<char*>(&layer.visible), sizeof(layer.visible));
        syncFile.read(reinterpret_cast<char*>(&layer.editing), sizeof(layer.editing));
        syncFile.read(reinterpret_cast<char*>(&layer.selected), sizeof(layer.selected));
        syncFile.read(reinterpret_cast<char*>(&layer.id), sizeof(layer.id));
        syncFile.read(reinterpret_cast<char*>(&layer.opacity), sizeof(layer.opacity));

    }

    sync.layerTextures.resize(layerSize);

    int size = sync.canvasWidth * sync.canvasHeight * 4;
    for (auto& item : sync.layerTextures)
    {
        std::vector<unsigned char> pixels(size);
        syncFile.read(reinterpret_cast<char*>(pixels.data()), size);
        item = pixels;
    }

    int folderSize;
    syncFile.read(reinterpret_cast<char*>(&folderSize), sizeof(folderSize));
    std::cout << "folderSize: " << folderSize << std::endl;
    sync.folders.resize(folderSize);

    for (auto& folder : sync.folders) {
        int nameLength;
        syncFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        folder.name.resize(nameLength);
        syncFile.read(&folder.name[0], nameLength);

        syncFile.read(reinterpret_cast<char*>(&folder.visible), sizeof(folder.visible));
        syncFile.read(reinterpret_cast<char*>(&folder.editing), sizeof(folder.editing));
        syncFile.read(reinterpret_cast<char*>(&folder.selected), sizeof(folder.selected));
        syncFile.read(reinterpret_cast<char*>(&folder.id), sizeof(folder.id));
        syncFile.read(reinterpret_cast <char*>(&folder.opacity), sizeof(folder.opacity));

        syncFile.read(reinterpret_cast<char*>(&folder.open), sizeof(folder.open));

        int childrenSize;
        syncFile.read(reinterpret_cast<char*>(&childrenSize), sizeof(childrenSize));
        folder.childrenIds.resize(childrenSize);
        syncFile.read(reinterpret_cast<char*>(folder.childrenIds.data()), sizeof(int) * childrenSize);
    }

    syncFile.close();
}

void SetRenderData()
{
    renderer->nodes.clear();

    unsigned int sizes[2] = {sync.canvasWidth, sync.canvasHeight};
    renderer->SetDrawData(sync.canvasWidth, sync.canvasHeight);

    int index = 0;
    for (Layer layer : sync.layers)
    {
        RenderData layerData;
        NewDraw::initLayer(layerData, sync.layerTextures[index]);
        renderer->nodes[layer.id] = std::make_unique<Layer>(layer.name,layer.id,layerData);
        renderer->layers.push_back(layer.id);
        index++;
    }

    for (Folder folder : sync.folders)
    {
        renderer->nodes[folder.id] = std::make_unique<Folder>(folder.name, folder.id);
        Folder* foldy = dynamic_cast<Folder*>(renderer->nodes[folder.id].get());
        for (int child : folder.childrenIds)
        {
            foldy->AddChild(child);
        }
        renderer->folders.push_back(folder.id);
    }
}

void DataManager::SaveSyncData(std::string path)
{
    int status = remove(path.c_str());
    if (status != 0) perror("Error deleting file");
    else std::cout << "File successfully deleted" << std::endl;

    SetSyncData();
    SaveSync(path);
}

void DataManager::LoadSyncData(std::string path)
{
    LoadSync(path);
    SetRenderData();
}