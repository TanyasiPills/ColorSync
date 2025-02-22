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

static bool showExplorer = true;

int counter = 0;

void Explorer::Init()
{
    folderTexture.Init("Resources/icons/folder.png");
    folderFullTexture.Init("Resources/icons/folderFull.png");
    fileTexture.Init("Resources/icons/file.png");
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

void Explorer::FileExplorerUI(bool* creatorStuff) {

    if (Lss::Modal("Explorer", &showExplorer, ImVec2(50 * Lss::VW, 40 * Lss::VH), Centered))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
        ImGui::Columns(7, nullptr, false);


        size_t pos = currentPath.find_last_of("\\");
        if (pos != std::string::npos && pos > 2) {
            std::string backPath = currentPath.substr(0, pos);
            bool isBack = (".." == selectedFile);

            if (ImGui::Selectable("##..", isBack, 0, ImVec2(0, iconSize.y + 2 * Lss::VH))) {
                selectedFile = "..";
                std::cout << "Selected: " << " .." << std::endl;
            }
            ImVec2 pos = ImGui::GetItemRectMin();
            ImVec2 size = ImGui::GetItemRectSize();
            ImVec2 cursorPos = ImVec2(pos.x + size.x / 2 - iconSize.x / 2, pos.y);
            ImGui::SetCursorScreenPos(cursorPos);

            Lss::Image(folderFullTexture.GetId(), iconSize);
            Lss::Text("..", 2 * Lss::VH);

            if (isBack && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                std::cout << "Changed path to: " << backPath << std::endl;
                currentPath = backPath;
                selectedFile.clear();
            }
            Lss::End();
            ImGui::NextColumn();
        }


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

         
            if (ImGui::Selectable(("##"+file).c_str(), isSelected, 0, ImVec2(0, iconSize.y+2*Lss::VH))) {
                selectedFile = file;
                std::cout << "Selected: " << file << std::endl;
            }
            ImVec2 pos = ImGui::GetItemRectMin();
            ImVec2 size = ImGui::GetItemRectSize();
            ImVec2 cursorPos = ImVec2(pos.x + size.x / 2 - iconSize.x/2, pos.y);
            ImGui::SetCursorScreenPos(cursorPos);

            Lss::Image(icon, iconSize);
            Lss::Text(file, 2 * Lss::VH);

            if (isSelected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isDirectory)
            {
                std::cout << "Changed path to: " << newPath << std::endl;
                currentPath = newPath;
                selectedFile.clear();
            }
            Lss::End();
            ImGui::NextColumn();         
        }
        ImGui::PopItemFlag();
        counter = 0;
        ImGui::EndPopup();
    }
    else {
        *creatorStuff = false;
        showExplorer = true;
    }
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