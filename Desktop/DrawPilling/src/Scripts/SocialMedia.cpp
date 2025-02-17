#include "SocialMedia.h"
#include <iostream>
#include <thread>
#include <map>
#include "lss.h"
#include "CallBacks.h"

std::vector<Post> SocialMedia::posts = {};
int SocialMedia::lastId = 0;
std::queue<std::tuple<std::vector<uint8_t>, Post*, int>> textureQueue;
std::mutex textureQueueMutex;

float startY, endY;
bool canGet = false;
float prevScrollY;

int mode = 1; // 0 - social, 1 - settings, 2 - search, ...

void SocialMedia::ProcessThreads()
{
    std::lock_guard<std::mutex> lock(textureQueueMutex);
    while (!textureQueue.empty()) {
        std::tuple<std::vector<uint8_t>, Post*, int> front = textureQueue.front();
        textureQueue.pop();
        std::vector<uint8_t> imageData = std::get<0>(front);
        Post* post = std::get<1>(front);
        int type = std::get<2>(front);
        switch (type)
        {
        case 1:
            post->image = HManager::ImageFromRequest(imageData, post->ratio);
            post->picLoaded = true;
            if (post->picLoaded && post->pPicLoaded) post->allLoaded = true;
            break;
        case 2: {
            float ratioAF = 0.0f;
            post->userImage = HManager::ImageFromRequest(imageData, ratioAF);
            post->pPicLoaded = true;
            if (post->picLoaded && post->pPicLoaded) post->allLoaded = true;
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
        Lss::SetColor(ContainerBackground, ContainerBackground);
        ImGui::GetStyle().ChildBorderSize = 0.0f;
        ImVec2 valid = ImGui::GetContentRegionAvail();
        Lss::Child("Feed", ImVec2(valid.x, 0), true, Centered); //ImGuiWindowFlags_NoScrollbar);

        float scrollY = ImGui::GetScrollY();
        float scrollMaxY = ImGui::GetScrollMaxY();

        if ((scrollY / scrollMaxY) > 0.95f && canGet) {
            if (prevScrollY != scrollY && scrollY > prevScrollY) {
                prevScrollY = scrollY;
                canGet = false;
                GetPosts();
            }

        }
        if ((scrollY / scrollMaxY) < 0.90f) {
            canGet = true;
        }

        for (Post& post : posts)
        {
            if (!post.allLoaded) continue;
            int validWidth = width * 0.9f;
            std::string id = std::to_string(post.id);
            Lss::Child("##" + id, ImVec2(validWidth, post.size), true, Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            if (post.size == 0) startY = ImGui::GetCursorPosY();
            Lss::Image(post.userImage, ImVec2(8 * Lss::VH, 8 * Lss::VH));
            ImGui::SameLine();
            Lss::Top(2 * Lss::VH);
            Lss::Text(post.username, 4 * Lss::VH);
            Lss::Left(Lss::VH);
            Lss::Text(post.text, 3 * Lss::VH);
            float good = validWidth * 0.9f;
            ImVec2 faki(good, good * post.ratio);
            Lss::Image(post.image, faki, Centered);
            if (!post.comments.empty())
            {
                if (ImGui::TreeNodeEx("Comments", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    float cornerRadius = 10.0f;
                    Lss::SetColor(ContainerBackground, Background);
                    ImVec2 commentChildSize = ImVec2(0, 200);
                    ImVec2 commentPos = ImGui::GetCursorScreenPos();

                    ImGui::BeginChild("CommentsRegion", commentChildSize, true);
                    drawList->AddRectFilled(commentPos, ImVec2(commentPos.x + commentChildSize.x, commentPos.y + commentChildSize.y),
                        IM_COL32(40, 40, 40, 255), cornerRadius);
                    for (Comment& comment : post.comments)
                    {
                        Lss::Top(Lss::VH);
                        Lss::Image(post.userImage, ImVec2(6 * Lss::VH, 6 * Lss::VH));
                        ImGui::SameLine();
                        Lss::Top(1 * Lss::VH);
                        Lss::Text(comment.username, 4 * Lss::VH);
                        Lss::Text(comment.text, 3 * Lss::VH);
                        ImGui::Separator();
                    }
                    Lss::End();
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
            }
            if (post.size == 0) {
                endY = ImGui::GetCursorPosY();
                post.size = endY - startY;
            }
            Lss::End();
            ImGui::EndChild();

            ImGui::Separator();
        }
        ImGui::EndChild();
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
        char ipText[128] = "";
        Lss::InputText("faku", ipText, sizeof(ipText), ImVec2(25 * Lss::VH, 2.5f * Lss::VH), Rounded);

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
        static int myInt = 0;
        if (myInt >= 100) myInt = 99;
        Lss::InputInt("faku", &myInt, ImVec2(4.2f * Lss::VH, 2.5f * Lss::VH), Rounded);

        Lss::End();
        ImGui::EndChild();


        Lss::End();
        ImGui::EndChild();
        } break;
    case 2: //search
        break;
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
        //GetPosts();
    }
    Lss::Top(1 * Lss::VH);
    if (Lss::Button("profile", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
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
    std::thread([]() {
        std::cout << "NewLastId: " << lastId << std::endl;;
        nlohmann::json jsonData = HManager::Request(("25.16.177.252:3000/posts?lastId=" + std::to_string(lastId)+"&take=10").c_str(), "", GET);
        std::cout << "Data: " << jsonData["data"].size() << std::endl;
        if (jsonData["newLastId"].is_null()) return;
        for (const auto& postJson : jsonData["data"]) {
            Post post;
            post.id = postJson["id"];
            post.userId = postJson["user"]["id"];
            if (postJson["imageId"].is_null()) post.imageId = -1;
            else post.imageId = postJson["imageId"];
            post.text = postJson["text"];
            post.username = postJson["user"]["username"];
            post.time = ParsePostTime(postJson["date"]);

            for (const auto& commentJson : postJson["comments"]) {
                Comment comment;
                comment.id = commentJson["id"];
                comment.userId = commentJson["user"]["id"];
                comment.text = commentJson["text"];
                comment.username = commentJson["user"]["username"];
                comment.time = ParsePostTime(commentJson["date"]);

                post.comments.push_back(comment);
            }

            posts.push_back(post);
        }

        lastId = jsonData["newLastId"];

    }).join();
    LoadImages();
}
void SocialMedia::LoadImages() 
{
    std::mutex mtx;

    for (Post& post : posts) {
        std::thread(&SocialMedia::LoadImageJa, &post, 2).detach();
        std::thread(&SocialMedia::LoadImageJa, &post, 1).detach();
    }
}


void SocialMedia::LoadImageJa(Post* post, int type)
{
    std::vector<uint8_t> imageData;
    switch (type) {
    case 1: {
        if (post->imageId == -1)return;
        imageData = HManager::Request(("25.16.177.252:3000/images/public/" + std::to_string(post->imageId)).c_str(), GET);
        } break;
    case 2: {
        imageData = HManager::Request(("25.16.177.252:3000/users/" + std::to_string(post->userId) + "/pfp").c_str(), GET);
        } break;
    default:
        break;
    }
    std::lock_guard<std::mutex> queueLock(textureQueueMutex);
    textureQueue.push(std::make_tuple(std::move(imageData), post, type));
}
