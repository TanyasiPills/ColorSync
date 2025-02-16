#include "SocialMedia.h"
#include <iostream>
#include <thread>
#include <map>
#include "lss.h"

std::vector<Post> SocialMedia::posts = {};
int SocialMedia::lastId = 0;
NewRenderer* renderer;

void SocialMedia::Init(NewRenderer& rendererIn)
{
    renderer = &rendererIn;
}

void SocialMedia::MainFeed(float position, float width, float height)
{
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    Lss::SetColor(Background, ContainerBackground);
    ImGui::Begin("Main Feed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::GetStyle().ChildBorderSize = 0.0f;
    ImVec2 valid = ImGui::GetContentRegionAvail();
    Lss::Child("Feed", ImVec2(valid.x, 0), true, Centered); //ImGuiWindowFlags_NoScrollbar);/
    for (Post post : posts)
    {
        valid = ImGui::GetContentRegionAvail();
        Lss::Child("##" + post.id, ImVec2(valid.x, 0), true, Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        Lss::Image(post.userImage, ImVec2(4*Lss::VH, 4*Lss::VH));
        Lss::Image(post.image, ImVec2(valid.x, post.ratio * valid.x), Centered);
        ImGui::EndChild();

        ImGui::Separator();
        
    }
    ImGui::EndChild();

    ImGui::End();
    Lss::SetColor(Background, Background);
}
void SocialMedia::LeftSide(float position, float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(position, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Left Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    float originalFontSize = ImGui::GetFont()->FontSize;

    Lss::Text("ColorSync", 5 * Lss::VH, Centered);

    ImGui::Separator();
    Lss::Top(2 * Lss::VH);
    if (Lss::Button("Heooo", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)){
        SocialMedia::GetPosts();
        SocialMedia::LoadImages();
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
        nlohmann::json jsonData = HManager::Request(("25.16.177.252:3000/posts?lastId=" + std::to_string(lastId) + "&take=1").c_str(), "", GET);

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
}
void SocialMedia::LoadImages() 
{
    std::mutex mtx;
    std::vector<std::thread> threads;

    for (Post& post : posts) {
        threads.push_back(std::thread(&SocialMedia::LoadImageJa, &post, std::ref(mtx), 1));
    }
    for (auto& thread : threads) {
        thread.join(); 
    }
}


void SocialMedia::LoadImageJa(Post* post, std::mutex& mtx,int type)
{
    std::vector<uint8_t> imageData;
    switch (type) {
    case 1: {
        std::lock_guard<std::mutex> lock(mtx);
        imageData = HManager::Request(("25.16.177.252:3000/images/public/" + std::to_string(post->imageId)).c_str(), GET);
        std::lock_guard<std::mutex> queueLock(renderer->textureQueueMutex);
        renderer->textureQueue.push(std::make_tuple(std::move(imageData), post,1));
        } break;
    case 2: {
        std::lock_guard<std::mutex> lock(mtx);
        imageData = HManager::Request(("25.16.177.252:3000/users/" + std::to_string(post->userId) + "/pfp").c_str(), GET);
        std::lock_guard<std::mutex> queueLock(renderer->textureQueueMutex);
        renderer->textureQueue.push(std::make_tuple(std::move(imageData), post, 2));
        } break;
    default:
        break;
    }
}
