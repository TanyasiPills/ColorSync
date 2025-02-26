#include "SocialMedia.h"
#include <iostream>
#include <thread>
#include <map>
#include <unordered_map>
#include "lss.h"
#include "CallBacks.h"
#include "RuntimeData.h"
#include "FIleExplorer.h"

std::vector<Post> SocialMedia::posts = {};
std::unordered_map<int, User> users;
std::unordered_map<int, GLuint> profilePics;
int SocialMedia::lastId = 0;
std::queue<std::tuple<std::vector<uint8_t>, int, int>> textureQueue;
std::mutex textureQueueMutex;

float startY, endY;
bool canGet = false;
bool init = true;
float prevScrollY;
bool creatingPost = false;

static RuntimeData& runtime = RuntimeData::getInstance();

int mode = 0; // 0 - social, 1 - settings, 2 - search, ...


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
            if (dataId> posts.size()-1) std::cout << "�isg�akhrg�aiihr�ana�ig" << std::endl;
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
        default:
            break;
        }
    }
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
        ImGui::GetStyle().ChildBorderSize = 0.0f;
        ImVec2 valid = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosY(0);
        Lss::Child("Feed", ImVec2(valid.x, 0), true, Centered); //ImGuiWindowFlags_NoScrollbar);

        float scrollY = ImGui::GetScrollY();
        float scrollMaxY = ImGui::GetScrollMaxY();

        if (((scrollY / scrollMaxY) > 0.95f && canGet) || init) {
            if (prevScrollY != scrollY && scrollY > prevScrollY) {
                prevScrollY = scrollY;
                canGet = false;
                std::thread(&SocialMedia::GetPosts).detach();
            }
            if (init) {
                std::thread(&SocialMedia::GetPosts).detach();
                init = false;
            }

        }
        if ((scrollY / scrollMaxY) < 0.90f) {
            canGet = true;
        }
        
        for (Post& post : posts)
        {
            if (!post.allLoaded) {
                if (post.picLoaded && users[post.userId].pPicLoaded) post.allLoaded = true;
                continue;
            }
            bool needChange = false;
            int validWidth = width * 0.9f;
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

        Lss::Child("FixedButton", ImVec2(0,0), false, 0, ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

        if (Lss::Button("Post", ImVec2(12*Lss::VH, 5*Lss::VH), 5*Lss::VH, Rounded | Centered)) {
            creatingPost = true;
        }
        Lss::End();
        ImGui::EndChild();
        if (creatingPost) {
            Explorer::FileExplorerUI(&creatingPost);
            mode = 3;
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
        static bool openStuff = true;
        static bool created = false;

        if (Lss::Modal("Sup", &openStuff,ImVec2(20*Lss::VW,50*Lss::VH),Centered | Trans, ImGuiWindowFlags_NoDecoration))
        {
            ImVec2 valid = ImGui::GetContentRegionAvail();
            Lss::Text("What's on your ming?", 2 * Lss::VH);
            static char inputtext[128] = "";
            Lss::InputText("Heoooo", inputtext, sizeof(inputtext), ImVec2(20 * Lss::VH, 2 * Lss::VH), Centered | Trans);
            Lss::Top(-Lss::VH/2);
            Lss::Separator(1.0f, 20 * Lss::VH, 4, Centered);

            ImVec2 addFileButton = ImVec2(12 * Lss::VH, 4 * Lss::VH);
            if (Lss::Button("Add File", addFileButton, 4 * Lss::VH)) {
                created = true;
            }
            Lss::End();

            if(created) Explorer::FileExplorerUI(&created);

            ImVec2 buttonSize = ImVec2(100, 20);
            ImGui::SetCursorPosY(valid.y - buttonSize.y - 2 * Lss::VH);
            Lss::Button("Post##postButton", ImVec2(10*Lss::VH, 4*Lss::VH),3*Lss::VH, Centered);
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
    default:
        break;
    }
    

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
    if (Lss::Button("Editor", ImVec2(16 * Lss::VH, 6 * Lss::VH), 5 * Lss::VH, Invisible | Centered | Rounded)){
        Callback::EditorSwapCallBack();
    }
    Lss::Top(1 * Lss::VH);
    if (Lss::Button("Lobbies", ImVec2(16 * Lss::VH, 6 * Lss::VH), 5 * Lss::VH, Invisible | Centered | Rounded)) {
        //GetPosts();
    }
    Lss::Top(2 * Lss::VH);
    ImGui::Separator();
    Lss::Top(2 * Lss::VH);
    if (Lss::Button("Message", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
        //GetPosts();
    }
    Lss::Top(1 * Lss::VH);
    if (Lss::Button("Search", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
        if (mode != 2) mode = 2;
        else mode = 0;
    }
    Lss::Top(1 * Lss::VH);
    if (Lss::Button("Profile", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
        //GetPosts();
    }
    Lss::Top(40 * Lss::VH);
    if (Lss::Button("Settings", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Rounded)) {
        if (mode != 1) mode = 1;
        else mode = 0;
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
    nlohmann::json jsonData = HManager::Request((runtime.ip+":3000/posts?lastId=" + std::to_string(lastId)+"&take=10").c_str(), "", GET);
    if (jsonData["data"] == 0) {
        init = true;
        return;
    }
    if (jsonData["newLastId"].is_null()) return;
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

    lastId = jsonData["newLastId"];

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

    LoadImageJa(post.imageId, 1, index);
}


void SocialMedia::LoadImageJa(int dataId, int type, int postId)
{
    std::vector<uint8_t> imageData;
    switch (type) {
    case 1: {
        if (dataId == -1)return;
        imageData = HManager::Request((runtime.ip+":3000/images/public/" + std::to_string(dataId)).c_str(), GET);
        std::cout << "Post Id: " << postId << std::endl;
        dataId = postId;
        } break;
    case 2: {
            if (profilePics.find(dataId) != profilePics.end()) break;
            else {
                imageData = HManager::Request((runtime.ip+":3000/users/" + std::to_string(dataId) + "/pfp").c_str(), GET);
            }
        } break;
    default:
        break;
    }
    std::lock_guard<std::mutex> queueLock(textureQueueMutex);
    textureQueue.push(std::make_tuple(std::move(imageData), dataId, type));
}
