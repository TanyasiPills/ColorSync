#include "FileExplorer.h"
#include "lss.h"
#include "Texture.h"

static std::string currentPath = "C:\\";
static std::string selectedFile;
GLuint folder;
GLuint folderFull;
GLuint fileId;

MyTexture folderTexture;
MyTexture folderFullTexture;
MyTexture fileTexture;

const ImVec2 iconSize(50, 50);

void Explorer::Init()
{
    folderTexture.Init("Resources/icons/folder.png");
    folderFullTexture.Init("Resources/icons/folderFull.png");
    fileTexture.Init("Resources/icons/file.png");

    std::cout << "Folder texture ID: " << folderTexture.GetId() << std::endl;
    std::cout << "File texture ID: " << fileTexture.GetId() << std::endl;
}

bool IsFolderEmpty(const std::string& folderPath) {
    std::string searchPath = folderPath + "\\*"; 
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
        return true; 

    bool hasFiles = false;
    do {
        std::string fileName = findData.cFileName;
        if (fileName != "." && fileName != "..") {
            hasFiles = true;
            break;
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    return !hasFiles;
}

void Explorer::FileExplorerUI() {

    ImGui::Begin("Explorer");
    ImGui::Columns(4, nullptr, false);
    /*
    ImGui::Text("Current Directory: %s", currentPath.c_str());
    if (ImGui::Button("Up")) {
        size_t pos = currentPath.find_last_of("\\");
        if (pos != std::string::npos && pos > 2) {
            currentPath = currentPath.substr(0, pos);
        }
    }*/

    for (const auto& file : GetFilesInDirectory(currentPath)) {
        std::string newPath = currentPath + "\\" + file;
        DWORD attributes = GetFileAttributes(newPath.c_str());
        bool isDirectory = (attributes & FILE_ATTRIBUTE_DIRECTORY);
        bool isSelected = (file == selectedFile);


        ImTextureID icon;
        if (isDirectory) {
            if (IsFolderEmpty(newPath)) {
                icon = folderTexture.GetId();
            }
            else {
                icon = folderFullTexture.GetId();
            }
        }
        else {
            icon = fileTexture.GetId();
        }

        Lss::Image(icon, iconSize);
        if (ImGui::Selectable(file.c_str(), isSelected, 0, ImVec2(iconSize.x, 0))) {
            selectedFile = file;
            std::cout << "Selected: " << file << std::endl;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isDirectory) {
            std::cout << "Changed path to: " << newPath << std::endl;
            currentPath = newPath;
            selectedFile.clear();
        }
        Lss::End();
        ImGui::NextColumn();
    }
    ImGui::End();
}

std::vector<std::string> Explorer::GetFilesInDirectory(const std::string& directory) {
    std::vector<std::string> files;
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((directory + "\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) return files;

    do {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            files.push_back(findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData));
    FindClose(hFind);

    return files;
}