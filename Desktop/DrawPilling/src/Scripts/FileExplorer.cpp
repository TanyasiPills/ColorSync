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

std::vector<std::string> driveList;
std::vector<const char*> items;

void LoadDrives() {
    driveList.clear();
    items.clear();

    char driveStrings[256];
    DWORD size = GetLogicalDriveStringsA(sizeof(driveStrings), driveStrings);

    if (size == 0) {
        std::cerr << "Failed to get drives. Error: " << GetLastError() << std::endl;
        return;
    }

    for (char* drive = driveStrings; *drive; drive += strlen(drive) + 1) {
        std::string driveName(drive);
        driveName.pop_back();
        driveList.push_back(driveName);
    }
    for (const auto& drive : driveList) {
        items.push_back(drive.c_str());
    }
}

bool HasSpecificExtension(const std::string& file, const std::string& ext) {
    size_t pos = file.find_last_of(".");
    if (pos == std::string::npos) return false;
    return file.substr(pos) == ext;
}

void Explorer::Init()
{
    folderTexture.Init("Resources/icons/folder.png");
    folderFullTexture.Init("Resources/icons/folderFull.png");
    fileTexture.Init("Resources/icons/file.png");
    LoadDrives();
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

    if (Lss::Modal("Explorer", &showExplorer, ImVec2(60 * Lss::VW, 40 * Lss::VW), Centered))
    {
        const char* formats[] = { ".jpg", ".png", "all image - jpg/png", ".sync", };
        static const char* currentFormat = formats[0];
        static char tempPath[200] = "";
        static const char* currentItem = items[0];
        Lss::Text("Path: ", 4 * Lss::VH);
        ImGui::SameLine();
        Lss::SetFontSize(4 * Lss::VH);
        ImGui::SetNextItemWidth(4 * Lss::VW);
        bool check = ImGui::BeginCombo("##drive", currentItem, ImGuiComboFlags_NoArrowButton);
        if (check)
        {
            for (int n = 0; n < items.size(); n++)
            {
                bool is_selected = (currentItem == items[n]);
                if (ImGui::Selectable(items[n], is_selected)) {
                    currentItem = items[n];
                    std::strcpy(tempPath, "");
                    currentPath = std::string(items[n]) + "\\";
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (Lss::InputText("#actualPath", tempPath, sizeof(tempPath), ImVec2(20 * Lss::VW, 4 * Lss::VH), None, ImGuiInputTextFlags_EnterReturnsTrue)) 
        {
            std::string newPath(tempPath);
            std::replace(newPath.begin(), newPath.end(), '/', '\\');
            std::string finalPath = currentItem + newPath;
            if (PathFileExistsA(finalPath.c_str()))
            {
                std::cout << "yes" << std::endl;
                currentPath = finalPath;
                std::strcpy(tempPath, newPath.c_str());
            }
        }

        Lss::Child("view",ImVec2(40*Lss::VW, 40*Lss::VH));

        
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
            bool isHidden = (attributes & FILE_ATTRIBUTE_HIDDEN);
            if (isHidden) continue; 
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
        ImGui::Columns(1);
        Lss::Left(Lss::VW);
        Lss::Text("File:",4*Lss::VH);
        ImGui::SameLine();
        static char fileName[200];
        Lss::InputText("##FileName", fileName, sizeof(fileName), ImVec2(27 * Lss::VW, 4 * Lss::VH));
        ImGui::SameLine();
        Lss::SetFontSize(4 * Lss::VH);
        ImGui::SetNextItemWidth(10 * Lss::VW);
        bool formatChange = ImGui::BeginCombo("##format", currentFormat, ImGuiComboFlags_NoArrowButton);
        if (formatChange)
        {
            for (int n = 0; n < items.size(); n++)
            {
                bool is_selected = (currentFormat == formats[n]);
                if (ImGui::Selectable(formats[n], is_selected)) {
                    currentFormat = formats[n];
                    std::strcpy(fileName, "");
                    currentFormat = formats[n];
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        Lss::End();
        ImGui::EndChild();

        Lss::End();
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