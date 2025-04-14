#include "FileExplorer.h"
#include "lss.h"
#include "Texture.h"
#include "HighsManager.h"
#include <vector>

std::vector<FolderSave> favorites;
std::vector<FolderSave> recent;

std::vector<FolderSave>* Explorer::GetFavorites() {
    return &favorites;
}

std::vector<FolderSave>* Explorer::GetRecents() {
    return &recent;
}

void Explorer::SetFavorites(std::vector<FolderSave> fvs) {
    favorites = fvs;
}
void Explorer::SetRecents(std::vector<FolderSave> rcnts) {
    recent = rcnts;
}

const char* formats[] = {".jpg", ".png", "All recognized - jpg/png", ".sync"};
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
bool fileAleardyExists = false;

std::vector<std::string> driveList;
std::vector<const char*> items;

std::string imagePath = "";
static char tempPath[200] = "";

void Explorer::ResetPath() {
    imagePath = "";
}

std::string Explorer::GetImagePath()
{
    return imagePath;
}

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
    if (pos == std::string::npos || pos == 0) return false;
    //if (file.find(" ") != std::string::npos) return false;
    return file.substr(pos) == ext;
}

void Explorer::Init()
{
    folderTexture.Init("Resources/icons/folder.png");
    folderFullTexture.Init("Resources/icons/folderFull.png");
    fileTexture.Init("Resources/icons/file.png");
    LoadDrives();
}

bool Explorer::Exists()
{
    return fileAleardyExists;
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

void SearchBar()
{
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

    float widthAvail = ImGui::GetContentRegionAvail().x;

    if (Lss::InputText("#actualPath", tempPath, sizeof(tempPath), ImVec2(widthAvail-2*Lss::VW, 8 * Lss::VH), None, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::string newPath(tempPath);
        std::replace(newPath.begin(), newPath.end(), '/', '\\');
        std::string finalPath = currentItem + std::string(2, '\\') + newPath;
        if (PathFileExistsA(finalPath.c_str()))
        {
            currentPath = finalPath;
            std::strcpy(tempPath, newPath.c_str());
        }
    }
    ImGui::SameLine();
    if (Lss::Button("Fav", ImVec2(6 * Lss::VW, 4.7f * Lss::VH),4*Lss::VH)) {
        if (currentPath.size() > 3)
        {
            size_t lastSlash = currentPath.find_last_of("\\/");
            std::string lastFolder = currentPath.substr(lastSlash + 1);
            favorites.emplace_back(lastFolder, currentPath);
        }
    }
}

bool ValidFormat(int& currentId, std::string& newPath) 
{
    switch (currentId)
    {
    case 0:
        if (!HasSpecificExtension(newPath, ".jpg")) return false;
        break;
    case 1:
        if (!HasSpecificExtension(newPath, ".png")) return false;
        break;
    case 2:
        if (!(HasSpecificExtension(newPath, ".jpg") || HasSpecificExtension(newPath, ".png"))) return false;
        break;
    case 3:
        if (!HasSpecificExtension(newPath, ".sync")) return false;
        break;
    default:
        break;
    }
    return true;
}

void SideBar(float& childHeight)
{
    Lss::Child("sideBar", ImVec2(10 * Lss::VW, childHeight));
        Lss::Left(0.5f * Lss::VW);
        Lss::Text("Favorites", 2.5f * Lss::VH);
        Lss::Left(0.5f * Lss::VW);
        Lss::SetColor(ContainerBackground, Background);
        Lss::Child("favs", ImVec2(9 * Lss::VW, childHeight / 2 - 4 * Lss::VH));
            for (auto& child : favorites) {
                float valid = ImGui::GetContentRegionAvail().x;
                if (Lss::Button(child.name, ImVec2(valid, 2 * Lss::VH), 1.5f * Lss::VH, Invisible | Rounded)) {
                    currentPath = child.path;
                    std::string vmiPath = currentPath.substr(4);
                    strcpy(tempPath, vmiPath.c_str());
                }
                Lss::End();
            }
        ImGui::EndChild();
        Lss::LeftTop(0.5f * Lss::VW, 0.25f * Lss::VH);
        Lss::Text("Recent folders", 2.5f * Lss::VH);
        Lss::Left(0.5f * Lss::VW);
        Lss::Child("recents", ImVec2(9 * Lss::VW, childHeight / 2 - 4 * Lss::VH));
            for (auto& child : recent) {
                float valid = ImGui::GetContentRegionAvail().x;
                if (Lss::Button(child.name, ImVec2(valid, 2 * Lss::VH), 1.5f * Lss::VH, Invisible | Rounded)) {
                    currentPath = child.path;
                    std::string vmiPath = currentPath.substr(4);
                    strcpy(tempPath, vmiPath.c_str());
                }
                Lss::End();
            }
        ImGui::EndChild();
        Lss::SetColor(ContainerBackground, ContainerBackground);
        Lss::End();
    ImGui::EndChild();
}

void Explorer::FileExplorerUI(bool* creatorStuff, int idForFormat) {
    Lss::SetFontSize(1 * Lss::VH);

    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.1412f, 0.1412f, 0.3529f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.1412f, 0.1412f, 0.3529f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.1412f, 0.1412f, 0.3529f, 1.0f));
    ImVec4 currentBgColor = ImGui::GetStyle().Colors[ImGuiCol_PopupBg];
    ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = ImVec4(0.0627f, 0.0627f, 0.1451f, 1.0f);

    if (Lss::Modal("Explorer", &showExplorer, ImVec2(60 * Lss::VW, 40 * Lss::VW), Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        bool isHovered = false;
        static bool set = false;
        static const char* currentFormat = formats[2];
        if (idForFormat == 3 && !set) {
            currentFormat = formats[3];
            set = true;
        }
        static int currentId = idForFormat;
        static char fileName[200];

        SearchBar();

        ImVec2 padding = ImGui::GetStyle().FramePadding;
        float childHeight = 36 * Lss::VW - 2 * padding.y;
        SideBar(childHeight);
        ImGui::SameLine();

        Lss::Child("view", ImVec2(48.75f * Lss::VW, childHeight));
        ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);

        Lss::Left(Lss::VH);
        Lss::Text("Directories & Files:", 4 * Lss::VH);

        Lss::SetColor(ContainerBackground, Background);
        Lss::Left(Lss::VW);
        Lss::Child("#explorerView", ImVec2(46.75f * Lss::VW, childHeight - 11.5f * Lss::VH));
        Lss::Top(Lss::VH);
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
                if (backPath.size() > 3)
                {
                    std::string vmiPath = backPath.substr(4);
                    strcpy(tempPath, vmiPath.c_str());
                }
                else {
                    strcpy(tempPath, "");
                }
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
            if (!isDirectory) {
                if (!ValidFormat(currentId, newPath)) continue;
            }

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


            if (ImGui::Selectable(("##" + file).c_str(), isSelected, 0, ImVec2(0, iconSize.y + 2 * Lss::VH))) {
                selectedFile = file;
                std::cout << "Selected: " << file << std::endl;
                isHovered = true;
                strcpy(fileName, file.c_str());
            }
            ImVec2 pos = ImGui::GetItemRectMin();
            ImVec2 size = ImGui::GetItemRectSize();
            ImVec2 cursorPos = ImVec2(pos.x + size.x / 2 - iconSize.x / 2, pos.y);
            ImGui::SetCursorScreenPos(cursorPos);

            Lss::Image(icon, iconSize);
            Lss::SetFontSize(2 * Lss::VH);

            std::string fileNameStuff;
            if (file.size() > 10) fileNameStuff = file.substr(0, 10) + "...";
            else fileNameStuff = file;

            float nameSize = ImGui::CalcTextSize(fileNameStuff.c_str()).x;
            ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.x / 2) - (nameSize / 2), ImGui::GetCursorScreenPos().y));
            Lss::Text(fileNameStuff, 2 * Lss::VH);

            if (isSelected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isDirectory)
            {
                std::cout << "Changed path to: " << newPath << std::endl;
                std::string vmiPath = newPath.substr(4);
                strcpy(tempPath, vmiPath.c_str());
                currentPath = newPath;
                selectedFile.clear();
            }
            Lss::End();
            ImGui::NextColumn();
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !isHovered) {
            selectedFile = "";
        }
        ImGui::PopItemFlag();
        ImGui::Columns(1);

        Lss::End();
        ImGui::EndChild();
        Lss::SetColor(ContainerBackground, ContainerBackground);

        Lss::Top(Lss::VH);
        Lss::Left(Lss::VW);
        Lss::Text("File:", 4 * Lss::VH);
        ImGui::SameLine();
        if (Lss::InputText("##FileName", fileName, sizeof(fileName), ImVec2(30 * Lss::VW, 6 * Lss::VH), 0, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::cout << currentPath + fileName << std::endl;

        }
        ImGui::SameLine();
        Lss::SetFontSize(3 * Lss::VH);
        ImGui::SetNextItemWidth(10 * Lss::VW);
        int numFormats = sizeof(formats) / sizeof(formats[0]);
        bool formatChange = ImGui::BeginCombo("##format", currentFormat, ImGuiComboFlags_NoArrowButton);
        if (formatChange)
        {
            for (int n = 0; n < numFormats; n++)
            {
                bool is_selected = (currentFormat == formats[n]);
                if (ImGui::Selectable(formats[n], is_selected)) {
                    currentFormat = formats[n];
                    std::strcpy(fileName, "");
                    currentFormat = formats[n];
                    currentId = n;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (Lss::Button("Select", ImVec2(5 * Lss::VW, 4 * Lss::VH), 3 * Lss::VH))
        {
            std::string imagePrePath = currentPath +'\\'+'\\' + fileName;
            if (ValidFormat(idForFormat, imagePrePath))
            {
                DWORD fileAttrib = GetFileAttributes(imagePrePath.c_str());
                if (fileAttrib != INVALID_FILE_ATTRIBUTES) {
                    fileAleardyExists = true;
                }
                else {
                    fileAleardyExists = false;
                }

                if (currentPath.size() > 3) {
                    size_t lastSlash = currentPath.find_last_of("\\/");
                    std::string lastFolder = currentPath.substr(lastSlash + 1);
                    recent.emplace_back(lastFolder, currentPath);
                }

                imagePath = imagePrePath;
                *creatorStuff = false;
            }
        }
        Lss::Top(Lss::VH);
        Lss::End();
        ImGui::EndChild();

    Lss::End();
    ImGui::EndPopup();
       
    }
    else {
        *creatorStuff = false;
        showExplorer = true;
    }

    ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = currentBgColor;
    ImGui::PopStyleColor(3);
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