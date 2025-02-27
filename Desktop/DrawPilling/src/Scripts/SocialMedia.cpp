#include "SocialMedia.h"
#include <iostream>
#include <thread>
#include <map>
#include <unordered_map>
#include "lss.h"
#include "CallBacks.h"
#include "RuntimeData.h"
#include "FIleExplorer.h"
#include "LoginRegister.h"

std::vector<Post> SocialMedia::posts = {};
std::unordered_map<int, User> users;
std::unordered_map<int, GLuint> profilePics;
int SocialMedia::lastId = 0;
std::queue<std::tuple<std::vector<uint8_t>, int, int>> textureQueue;
std::mutex textureQueueMutex;

std::vector<GLuint> userImages;

float startY, endY;
bool canGet = false;
bool init = true;
float prevScrollY;
bool creatingPost = false;

bool loginWindow = false;

MyTexture imageToPostTexture;
GLuint imageToPost = 0;

MyTexture userImageTexture;


static RuntimeData& runtime = RuntimeData::getInstance();

int mode = 0; // 0 - social, 1 - settings, 2 - search, ...


std::queue<std::tuple<std::vector<uint8_t>, int, int>>* SocialMedia::GetTextureQueue()
{
	return &textureQueue;
}

void SocialMedia::ProcessThreads()
{
    std::lock_guard<std::mutex> lock(textureQueueMutex);
    while (!textureQueue.empty()) {
        std::tuple<std::vector<uint8_t>, int, int> front = textureQueue.front();
        textureQueue.pop();
        std::vector<uint8_t> imageData = std::get<0>(front);
        int dataId = std::get<1>(front);
        int type = std::get<2>(front);

        switch (type)
        {
        case 1:
            posts[dataId].image = HManager::ImageFromRequest(imageData, posts[dataId].ratio);
            posts[dataId].picLoaded = true;
            break;
        case 2: {
            float ratioAF = 0.0f;
            GLuint profileImage;
            if (!imageData.empty()) {
                profileImage = HManager::ImageFromRequest(imageData, ratioAF);
                profilePics[dataId] = profileImage;
            }
            else profileImage = profilePics[dataId];

            users[dataId].userImage = profileImage;
            users[dataId].pPicLoaded = true;
            } break;
        case 3: {
                float ratioAF = 0.0f;
                runtime.pfpTexture = HManager::ImageFromRequest(imageData, ratioAF);
            } break;
        case 4:
            {
                float ratioAF = 0.0f;
                userImages.emplace_back(HManager::ImageFromRequest(imageData, ratioAF));
            } break;
        default:
            break;
        }
    }
}

void SocialMedia::LoadProfile()
{
    nlohmann::json jsonData = HManager::Request(("images/user/private/" + std::to_string(runtime.id)).c_str(), "", GET);
    if (jsonData.is_null()) {
        std::cout << "cant recieve profile image list" << std::endl;
        return;
    }

    std::vector<int> imageIds;
    for (const auto& imageJson : jsonData)
    {
        imageIds.emplace_back(imageJson["id"]);
    }

    for (int& image : imageIds)
    {
        std::vector<uint8_t> imageData = HManager::ImageRequest(("images/" + std::to_string(image)).c_str());
        std::lock_guard<std::mutex> queueLock(textureQueueMutex);
        textureQueue.push(std::make_tuple(std::move(imageData), 0, 4));
    }
}

void SocialMedia::ProfilePage(float& width, float& height) 
{
    static bool needImages = true;

    static bool imageEditOpen = false;
    static bool openExplorer = false;
    static bool wasOpenExplorer = false;

    static bool imageUploadpen = false;
    static bool openExplorer2 = false;
    static bool wasOpenExplorer2 = false;

    if (needImages) {
        std::thread(&SocialMedia::LoadProfile).detach();
        needImages = false;
    }

    ImVec2 valid = ImGui::GetContentRegionAvail();
    Lss::Child("Feed", ImVec2(valid.x, 0), false, Centered, ImGuiWindowFlags_NoScrollbar);
    int validWidth = width * 0.6f;
    Lss::Child("##user", ImVec2(validWidth, height), true, Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    //pfp change
    if (Lss::Modal("pfpChange", &imageEditOpen, ImVec2(20 * Lss::VW, 35 * Lss::VH), Centered | Trans, ImGuiWindowFlags_NoDecoration))
    {
        ImVec2 valid = ImGui::GetContentRegionAvail();
        Lss::Text("Change your avatar", 4 * Lss::VH, Centered);

        ImVec2 addFileButton = ImVec2(12 * Lss::VH, 4 * Lss::VH);
        if (Lss::Button("Add File", addFileButton, 4 * Lss::VH, Centered)) {
            openExplorer = true;
            wasOpenExplorer = true;
        }
        Lss::End();

        if (openExplorer) Explorer::FileExplorerUI(&openExplorer);
        else if (wasOpenExplorer) {
            std::string imagePath = Explorer::GetImagePath();
            if (imagePath.size() > 2) {
                userImageTexture.Init(imagePath);
                runtime.pfpTexture = userImageTexture.GetId();
                wasOpenExplorer = false;
            }
            else {
                wasOpenExplorer = false;
            }
        }
        if (userImageTexture.GetId() <= 0)
        {
            std::vector<uint8_t> imageData = HManager::ImageRequest(("users/" + std::to_string(runtime.id) + " / pfp").c_str());
            float ratioStuff = 0.0f;
            runtime.pfpTexture = HManager::ImageFromRequest(imageData, ratioStuff);
        }
        Lss::Image(runtime.pfpTexture, ImVec2(20 * Lss::VH, 20 * Lss::VH), Centered);
        ImVec2 buttonSize = ImVec2(100, 20);
        ImGui::SetCursorPosY(valid.y - buttonSize.y - 2 * Lss::VH);
        if (Lss::Button("Modify##modifyImage", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered))
        {
            std::string imagePath = Explorer::GetImagePath();
            if (imagePath.size() > 2) {
                std::string fileName = imagePath.substr(imagePath.find_last_of('\\') + 1);
                nlohmann::json jsonData = HManager::ImageUploadRequest(imagePath, 1);
                if (!jsonData.is_null()) imageEditOpen = false;
            }
        }


        if (!openExplorer && ImGui::IsMouseClicked(0))
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 cursorPos = ImGui::GetMousePos();
            ImVec2 size = ImGui::GetWindowSize();
            if (!Lss::InBound(cursorPos, pos, size)) {
                imageEditOpen = false;
                ImGui::CloseCurrentPopup();
            }
        }
        Lss::End();
        ImGui::EndPopup();
    }

    if (Lss::Modal("imageToGalery", &imageUploadpen, ImVec2(20 * Lss::VW, 35 * Lss::VH), Centered | Trans, ImGuiWindowFlags_NoDecoration))
    {
        ImVec2 valid = ImGui::GetContentRegionAvail();
        Lss::Text("Add file to galery", 4 * Lss::VH, Centered);

        ImVec2 addFileButton = ImVec2(12 * Lss::VH, 4 * Lss::VH);
        if (Lss::Button("Add File", addFileButton, 4 * Lss::VH, Centered)) {
            openExplorer2 = true;
            wasOpenExplorer2 = true;
        }
        Lss::End();

        if (openExplorer2) Explorer::FileExplorerUI(&openExplorer2);
        else if (wasOpenExplorer2) {
            wasOpenExplorer2 = false;
        }
        if (userImageTexture.GetId() > 0) Lss::Image(runtime.pfpTexture, ImVec2(20 * Lss::VH, 20 * Lss::VH), Centered);
        ImVec2 buttonSize = ImVec2(100, 20);
        ImGui::SetCursorPosY(valid.y - buttonSize.y - 2 * Lss::VH);
        if (Lss::Button("Add##addImage", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered))
        {
            std::string imagePath = Explorer::GetImagePath();
            if (imagePath.size() > 2) {
                nlohmann::json jsonData = HManager::ImageUploadRequest(imagePath, 0);
                if (!jsonData.is_null()) {
                    imageUploadpen = false;
                    needImages = true;
                }
            }
        }


        if (!openExplorer2 && ImGui::IsMouseClicked(0))
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 cursorPos = ImGui::GetMousePos();
            ImVec2 size = ImGui::GetWindowSize();
            if (!Lss::InBound(cursorPos, pos, size)) {
                imageUploadpen = false;
                ImGui::CloseCurrentPopup();
            }
        }
        Lss::End();
        ImGui::EndPopup();
    }
    Lss::Image(runtime.pfpTexture, ImVec2(20 * Lss::VH, 20 * Lss::VH), Rounded);
    if (ImGui::IsItemClicked()) {
        imageEditOpen = true;
    }
    ImGui::SameLine();
    Lss::Top(7.5 * Lss::VH);
    Lss::Text(runtime.username, 5 * Lss::VH);

    ImGui::SameLine();
    Lss::Top(7.5 * Lss::VH);
    Lss::Left(10 * Lss::VH);
    if (Lss::Button("Add image", ImVec2(20 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH))
    {
        imageUploadpen = true;
    }
    Lss::Top(2 * Lss::VH);
    Lss::Separator(2.0f);
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float xWidth = avail.x / 3;
    for (size_t i = 0; i < userImages.size(); i++)
    {
        ImGui::Image(userImages[i], ImVec2(xWidth, xWidth));

        if ((i + 1) % 3 != 0)
            ImGui::SameLine();
    }

    Lss::End();
    ImGui::EndChild();
    ImGui::EndChild();
}

void SocialMedia::MainFeed(float position, float width, float height)
{
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    Lss::SetColor(Background, ContainerBackground);
    ImGui::Begin("Main Feed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    switch (mode)
    {
    case 0:{ //home page, social media
        static bool openStuff = false;
        static bool wasCreated = false;
        static bool created = false;

        ImGui::GetStyle().ChildBorderSize = 0.0f;
        ImVec2 valid = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosY(0);
        Lss::Child("Feed", ImVec2(valid.x, 0), true, Centered); //ImGuiWindowFlags_NoScrollbar);

        float scrollY = ImGui::GetScrollY();
        float scrollMaxY = ImGui::GetScrollMaxY();

        if ((scrollY / scrollMaxY) < 0.90f) {
            canGet = true;
        }
        if (((scrollY / scrollMaxY) > 0.95f && canGet) || init) {
            if (prevScrollY != scrollY) {
                prevScrollY = scrollY;
                canGet = false;
                std::thread(&SocialMedia::GetPosts).detach();
            }
            if (init) {
                std::thread(&SocialMedia::GetPosts).detach();
                init = false;
            }

        }
        
        for (Post& post : posts)
        {
            if (!post.allLoaded) {
                if (post.picLoaded && users[post.userId].pPicLoaded) post.allLoaded = true;
                continue;
            }
            bool needChange = false;
            int validWidth = width * 0.6f;
            std::string id = std::to_string(post.id);
            Lss::Child("##" + id, ImVec2(validWidth, post.size), true, Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            if (post.size == 0) {
                startY = ImGui::GetCursorPosY();
                needChange = true;
            }
            Lss::Image(users[post.userId].userImage, ImVec2(8 * Lss::VH, 8 * Lss::VH), Rounded);
            ImGui::SameLine();
            Lss::Top(2 * Lss::VH);
            Lss::Text(users[post.userId].username, 4 * Lss::VH);
            Lss::Left(Lss::VH);
            Lss::Text(post.text, 3 * Lss::VH);
            float good = validWidth * 0.9f;
            ImVec2 faki(good, good * post.ratio);
            Lss::Image(post.image, faki, Centered);
            if (!post.comments.empty())
            {
                bool open = ImGui::TreeNodeEx("Comments", ImGuiTreeNodeFlags_DefaultOpen);
                /*
                if (post.openComments != open) {
                    post.size = 0;  
                    post.openComments = open;
                }*/

                if (open) {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    float cornerRadius = 10.0f;
                    ImVec2 commentChildSize;
                    if (post.comments.size() == 1) commentChildSize = ImVec2(ImGui::GetContentRegionAvail().x - 20, 11 * Lss::VH);
                    else commentChildSize = ImVec2(ImGui::GetContentRegionAvail().x-20, 20*Lss::VH);
                    ImVec2 commentPos = ImGui::GetCursorScreenPos();

                    ImGui::BeginChild("CommentsRegion", commentChildSize, true, ImGuiWindowFlags_NoScrollbar);
                    for (Comment& comment : post.comments)
                    {
                        if (!users[comment.userId].pPicLoaded) continue;

                        Lss::SetColor(ContainerBackground, Background);
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
                        std::string name = std::to_string(comment.id);
                        Lss::Child(name, ImVec2(0, 11 * Lss::VH));
                        Lss::Top(Lss::VH);
                        Lss::Left(Lss::VH);
                        Lss::Image(users[comment.userId].userImage, ImVec2(6 * Lss::VH, 6 * Lss::VH), Rounded);
                        ImGui::SameLine();
                        Lss::Top(Lss::VH);
                        Lss::Text(users[comment.userId].username, 4 * Lss::VH);
                        Lss::Left(7*Lss::VH);
                        Lss::Text(comment.text, 3 * Lss::VH);
                        Lss::End();
                        ImGui::EndChild();
                        ImGui::PopStyleVar(1);
                        Lss::SetColor(ContainerBackground, ContainerBackground);
                        if (post.comments[post.comments.size()-1].id != comment.id) {
                            Lss::Top(Lss::VH);
                        }
                    }
                    Lss::End();
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
            }
            if (post.size == 0 && needChange) {
                endY = ImGui::GetCursorPosY();
                post.size = endY - startY;
            }
            Lss::End();
            ImGui::EndChild();

            ImGui::Separator();
        }
        ImGui::EndChild();

        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
            viewport->WorkPos.y + viewport->WorkSize.y - 2*Lss::VH),
            ImGuiCond_Always, ImVec2(0.5f, 1.0f));

        Lss::Child("FixedButton", ImVec2(12*Lss::VH, 5 * Lss::VH), false, Centered, ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground);

        if (Lss::Button("Post", ImVec2(12*Lss::VH, 5*Lss::VH), 5*Lss::VH, Rounded | Centered)) {
            openStuff = true;
        }
        Lss::End();
        ImGui::EndChild();

        if (Lss::Modal("Sup", &openStuff, ImVec2(20 * Lss::VW, 50 * Lss::VH), Centered | Trans, ImGuiWindowFlags_NoDecoration))
        {
            ImVec2 valid = ImGui::GetContentRegionAvail();
            Lss::Text("What's on your ming?", 2 * Lss::VH);
            static char inputtext[128] = "";
            Lss::InputText("Heoooo", inputtext, sizeof(inputtext), ImVec2(18 * Lss::VW, 2 * Lss::VH), Centered | Trans);
            Lss::Top(-Lss::VH / 2);
            Lss::Separator(1.0f, 20 * Lss::VH, 4, Centered);

            ImVec2 addFileButton = ImVec2(12 * Lss::VH, 4 * Lss::VH);
            if (Lss::Button("Add File", addFileButton, 4 * Lss::VH)) {
                created = true;
                wasCreated = true;
            }
            Lss::End();

            if (created) Explorer::FileExplorerUI(&created);
            else if (wasCreated) {
                std::string imagePath = Explorer::GetImagePath();
                if (imagePath.size() > 2) {
                    imageToPostTexture.Init(imagePath);
                    imageToPost = imageToPostTexture.GetId();
                    wasCreated = false;
                }
                else {
                    wasCreated = false;
                }
            }
            if (imageToPost > 0) Lss::Image(imageToPost, ImVec2(20 * Lss::VW, 20 * Lss::VH));
            ImVec2 buttonSize = ImVec2(100, 20);
            ImGui::SetCursorPosY(valid.y - buttonSize.y - 2 * Lss::VH);
            if (Lss::Button("Post##postButton", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered))
            {
                std::string imagePath = Explorer::GetImagePath();
                nlohmann::json jsonData = HManager::PostRequest(inputtext, imagePath);
                if (jsonData.is_null()) {
                    std::cout << "couldn't post post" << std::endl;
                }
                else {
                    posts = {};
                    openStuff = false;
                    lastId = 0;
                    init = true;
                }
            }
            if (!created && ImGui::IsMouseClicked(0))
            {
                ImVec2 pos = ImGui::GetWindowPos();
                ImVec2 cursorPos = ImGui::GetMousePos();
                ImVec2 size = ImGui::GetWindowSize();
                if (!Lss::InBound(cursorPos, pos, size)) {
                    ImGui::CloseCurrentPopup();
                    openStuff = false;
                }
            }
            Lss::End();
            ImGui::EndPopup();
        }
        } break;
    case 1: { //settings
        ImVec2 valid = ImGui::GetContentRegionAvail();
        Lss::Child("Feed", ImVec2(valid.x, 0), false, Centered, ImGuiWindowFlags_NoScrollbar);

        Lss::Text("Network Variables", 5 * Lss::VH);
        ImGui::SameLine();
        Lss::Top(2.4f * Lss::VH);
        Lss::Separator(2.0f);
        

        Lss::Left(5 * Lss::VW);
        Lss::Child("##NetworkVariables", ImVec2(0, 4.5*Lss::VH), false, Centered, ImGuiWindowFlags_NoScrollbar);

        Lss::Text("Server IP: ", 3 * Lss::VH);
        ImGui::SameLine();
        Lss::Top(0.25f * Lss::VH);
        static char ipText[128] = "";
        if(ipText[0] == '\0') std::strcpy(ipText, runtime.ip.c_str());
        if (Lss::InputText("faku", ipText, sizeof(ipText), ImVec2(25 * Lss::VH, 2.5f * Lss::VH), Rounded, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::cout << "zsa\n";
            runtime.ip = ipText;
            std::thread(&HManager::InitUser).detach();
        }

        Lss::End();
        ImGui::EndChild();


        Lss::Text("Drawing Variables", 5 * Lss::VH);
        ImGui::SameLine();
        Lss::Top(2.4f * Lss::VH);
        Lss::Separator(2.0f);


        Lss::Left(5 * Lss::VW);
        Lss::Child("##DrawingVariables", ImVec2(0, 4.5f * Lss::VH), false, Centered, ImGuiWindowFlags_NoScrollbar);

        Lss::Text("Undo count: ", 3 * Lss::VH);
        ImGui::SameLine();
        Lss::Top(0.25f * Lss::VH);
        static int myInt = runtime.undoCount;
        if (myInt >= 100) myInt = 99;
        if (Lss::InputInt("faku2", &myInt, ImVec2(4.2f * Lss::VH, 2.5f * Lss::VH), Rounded))
        {
            runtime.undoCount = myInt;
        }

        Lss::End();
        ImGui::EndChild();


        Lss::End();
        ImGui::EndChild();
        } break;
    case 2: { //search
        ImVec2 valid = ImGui::GetContentRegionAvail();
        Lss::Child("Feed", ImVec2(valid.x, 0), false, Centered, ImGuiWindowFlags_NoScrollbar);


        static char searchText[128] = "";
        bool hihi = Lss::InputText("faku", searchText, sizeof(searchText), ImVec2(50 * Lss::VH, 5.0f * Lss::VH), Rounded | Centered, ImGuiInputTextFlags_EnterReturnsTrue);
        static bool search = false;
        static int count = 0;
        if (hihi || search) {
            search = true;
            Lss::Top(10 * Lss::VH);
            std::string searchtext = "Searching";
            for (size_t i = 100; i < count; i+= 100)searchtext += ".";
            Lss::Text(searchtext, 4 * Lss::VH, Centered);
            count++;
            if (count >= 401) {
                count = 0;
            }
        }

        Lss::End();
        ImGui::EndChild();
        } break;
    case 3: {
        ProfilePage(width, height);
        } break;
    default:
        break;
    }
    
    LoginRegister::Login(loginWindow);

    Lss::SetColor(Background, Background);

    ImGui::End();
}
void SocialMedia::LeftSide(float position, float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Left Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    Lss::Text("ColorSync", 8 * Lss::VH, Centered);
    if (ImGui::IsItemClicked()) mode = 0;

    ImGui::Separator();
    Lss::Top(2 * Lss::VH);
    if (Lss::Button("Editor", ImVec2(16 * Lss::VH, 6 * Lss::VH), 5 * Lss::VH, Invisible | Centered | Rounded)) {
        Callback::EditorSwapCallBack();
    }
    Lss::Top(1 * Lss::VH);
    if (Lss::Button("Lobbies", ImVec2(16 * Lss::VH, 6 * Lss::VH), 5 * Lss::VH, Invisible | Centered | Rounded)) {
        //GetPosts();
    }
    Lss::Top(2 * Lss::VH);
    ImGui::Separator();
    if (runtime.logedIn) {
        Lss::Top(2 * Lss::VH);
        if (Lss::Button("Message", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
            //GetPosts();
        }
    }

    Lss::Top(1 * Lss::VH);
    if (Lss::Button("Search", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
        if (mode != 2) mode = 2;
        else mode = 0;
    }

    if (runtime.logedIn) {
        Lss::Top(1 * Lss::VH);
        if (Lss::Button("Profile", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
            if (mode != 3) mode = 3;
            else mode = 0;
        }
    }
    else {
        if (Lss::Button("Login", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
            loginWindow = true;
        }
    }

    float windowHeight = ImGui::GetWindowHeight();
    windowHeight -= 6 * Lss::VH;
    ImGui::SetCursorPosY(windowHeight);
    if (Lss::Button("Settings", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Rounded)) {
        if (mode != 1) mode = 1;
        else mode = 0;
    }

    if (runtime.logedIn)
    {
        ImGui::SameLine();
        float windowWidth = ImGui::GetWindowWidth();
        windowWidth -= 16 * Lss::VH;
        ImGui::SetCursorPosX(windowWidth);
        if (Lss::Button("Logout", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Rounded))
        {
            runtime.username = "";
            runtime.password = "";
            runtime.token = "";
            runtime.logedIn = false;
        }
    }


    Lss::Back();
    Lss::End();
    ImGui::End();
}
void SocialMedia::RightSide(float position, float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Right Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::Button("Button A")) {
    }
    if (ImGui::Button("Button B")) {
    }

    ImGui::End();
}

std::chrono::system_clock::time_point SocialMedia::ParsePostTime(const std::string& timeData) 
{
    std::tm tm = {};
    std::istringstream ss(timeData);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

void SocialMedia::GetPosts() 
{
    if (runtime.ip[0] == '\0') {
        std::cout << "runtimeIp not set\n";
        init = true;
        return;
    }
    nlohmann::json jsonData = HManager::Request(("posts?offset=" + std::to_string(lastId)+"&take=10").c_str(), "", GET);
    if (jsonData["offset"].is_null()) {
        std::cout << "no data left to steal :c\n";
        init = false;
        return;
    }
    for (const auto& postJson : jsonData["data"]) {
        Post post;
        post.id = postJson["id"];
        post.userId = postJson["user"]["id"];
        if (users.find(post.userId) == users.end()) {
            User user;
            user.username = postJson["user"]["username"];
            user.userImage = -1;
            users[post.userId] = user;
        }
        if (postJson["imageId"].is_null()) post.imageId = -1;
        else post.imageId = postJson["imageId"];
        post.text = postJson["text"];
        post.time = ParsePostTime(postJson["date"]);
    
        for (const auto& commentJson : postJson["comments"]) {
            Comment comment;
            comment.id = commentJson["id"];
            comment.userId = commentJson["user"]["id"];
            comment.text = commentJson["text"];
            if (users.find(comment.userId) == users.end()) {
                User user;
                user.username = commentJson["user"]["username"];
                user.userImage = -1;
                users[comment.userId] = user;
            }
            comment.time = ParsePostTime(commentJson["date"]);
    
            post.comments.push_back(comment);
        }
        posts.push_back(post);
    }

    lastId = jsonData["offset"];
    LoadImages();
}
void SocialMedia::LoadImages() 
{
    for (int i = 0; i < posts.size(); i++)
    {
        std::thread(&SocialMedia::LoadDependencies, std::ref(posts[i]), i).detach();
    }
}

void SocialMedia::LoadDependencies(Post& post, int index) 
{
    LoadImageJa(post.userId, 2);

    for (Comment comment : post.comments) {
        LoadImageJa(comment.userId, 2);
    }
    if(post.imageId != -1) LoadImageJa(post.imageId, 1, index);
    else post.picLoaded = true;
}

void SocialMedia::LoadImageJa(int dataId, int type, int postId)
{
    std::vector<uint8_t> imageData;
    switch (type) {
    case 1: {
        if (dataId == -1)return;
        imageData = HManager::ImageRequest(("images/public/" + std::to_string(dataId)).c_str());
        dataId = postId;
        } break;
    case 2: {
            if (profilePics.find(dataId) != profilePics.end()) break;
            else {
                imageData = HManager::ImageRequest(("users/" + std::to_string(dataId) + "/pfp").c_str());
            }
        } break;
    default:
        break;
    }
    std::lock_guard<std::mutex> queueLock(textureQueueMutex);
    textureQueue.push(std::make_tuple(std::move(imageData), dataId, type));
}
