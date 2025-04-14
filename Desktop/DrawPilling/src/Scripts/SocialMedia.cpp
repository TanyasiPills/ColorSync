#include "SocialMedia.h"
#include "DrawUI.h"
#include <iostream>
#include <thread>
#include <map>
#include <sstream>
#include <unordered_map>
#include <array>
#include <chrono>
#include "lss.h"
#include "CallBacks.h"
#include "RuntimeData.h"
#include "FIleExplorer.h"
#include "LoginRegister.h"
#include "SocksManager.h"
#include <mutex>
#include <ctime>


std::mutex postMutex;

std::vector<Room> friendRooms;
std::vector<Room> rooms;
std::vector<Post> SocialMedia::posts = {};
std::unordered_map<int, User> users;
std::unordered_map<int, GLuint> profilePics;
std::map<int, Image, std::greater<int>> postImageRelation;
std::unordered_map<int, GLuint> images;
std::vector<int> searchedUser;
int SocialMedia::lastId = 0;
int SocialMedia::searchOffset = 0;
std::queue<std::tuple<std::vector<uint8_t>, int, int>> textureQueue;
std::mutex textureQueueMutex;

std::vector<GLuint> userImages;

float startY, endY;
bool canGet = false;
bool init = true;
float prevScrollY;
bool creatingPost = false;

bool loginWindow = false;

bool searched = false;
bool searchPostOpen = false;

bool needRooms = true;

int searchMode = 0;
int selectedUser = -1;
bool needImages = true;

static bool openStuff = false;

MyTexture imageToPostTexture;
GLuint imageToPost = 0;

Post searchedPost;

MyTexture userImageTexture;

static RuntimeData& runtime = RuntimeData::getInstance();

int mode = 0; // 0 - social, 1 - settings, 2 - search, ...

MyTexture notLikedTexture;
MyTexture likedTexture;
MyTexture commentTexture;

bool needError = true;

std::string CalcTime(std::chrono::system_clock::time_point time)
{
    auto now = std::chrono::system_clock::now();
    std::string outTime;
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - time);
    if (duration.count() < 60) {
        outTime = std::to_string(duration.count()) + " minutes ago";
    }
    else if (duration.count() < 24 * 60)
        outTime = std::to_string(duration.count() / 60) + " horse ago";
    else if (duration.count() <= 24 * 4 * 60) {
        outTime = std::to_string(duration.count() / (24 * 60)) + " days ago";
    }
    else {
        std::time_t tt = std::chrono::system_clock::to_time_t(time);
        std::tm* gmt = std::localtime(&tt);
        char buffer[50];
        std::strftime(buffer, sizeof(buffer), "%B %d", gmt);
        outTime = buffer;
    }
    return outTime;
}

std::unordered_map<GLuint, int> imageIndexes;

std::queue<std::tuple<std::vector<uint8_t>, int, int>>* SocialMedia::GetTextureQueue()
{
	return &textureQueue;
}

void SocialMedia::LoadTextures()
{
    notLikedTexture.Init("Resources/icons/notLiked.png");
    notLikedTexture.Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    likedTexture.Init("Resources/icons/liked.png");
    likedTexture.Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    commentTexture.Init("Resources/icons/comment.png");
    commentTexture.Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void ParseSearchText(const char* searchText, std::vector<std::string>& tags, std::string& text) {
    std::istringstream stream(searchText);
    std::string word;
    text.clear();
    tags.clear();

    while (stream >> word) {
        if (word[0] == '#' && word.length() > 1) {
            tags.push_back(word.substr(1));
        }
        else {
            if (!text.empty()) text += " ";
            text += word;
            tags.push_back(word);
        }
    }
}

void SocialMedia::SearchStuff(const char* searchText, int type)
{
    bool end = false;
    std::string text;
    std::vector<std::string> tags;
    ParseSearchText(searchText, tags, text);
    if (type == 1) {
        while (postImageRelation.size() < 21 && !end) {
            std::string searchTerm = "posts/search?";
            searchTerm += "q=" + text;
            searchTerm += "&offset=" + std::to_string(searchOffset);
            for (const auto& tag : tags) {
                searchTerm += "&tags=" + tag;
            }

            nlohmann::json result = HManager::Request(searchTerm, "", GET);

            if (result.is_null()) {
                std::cerr << "post search request failed" << std::endl;
            }
            else {
                for (const auto& imageData : result["data"]) postImageRelation[imageData["id"]].imageId = imageData["imageId"];

                for (const auto& pair : postImageRelation) {
                    std::thread(&SocialMedia::LoadImageJa, pair.second.imageId, 5, pair.first).detach();
                }
                if (result["offset"].is_null()) end = true;
                else searchOffset = result["offset"];
            }
        }
        searchOffset = 0;
        std::cout << "finished" << std::endl;
    }
    else if (type == 2)
    {
        nlohmann::json profileResult = HManager::Request("users/search?name=" + text, "", GET);
        if (profileResult.is_null()) {
            std::cerr << "search request failed" << std::endl;
        }
        else {
            for (const auto& profile : profileResult) {
                User user;
                user.username = profile["username"];
                users[profile["id"]] = user;
                searchedUser.emplace_back(profile["id"]);
                std::thread(&SocialMedia::LoadImageJa, profile["id"], 2, 0).detach();
            }
        }
    }
}

void SocialMedia::GetPostForSearch(const int& postId)
{
    nlohmann::json result = HManager::Request("posts/" + std::to_string(postId), "", GET);
    if (result.is_null()) {
        std::cerr << "search request failed" << std::endl;
    }
    else {
        searchedPost.id = result["id"];
        searchedPost.userId = result["user"]["id"];
        searchedPost.text = result["text"];
        searchedPost.time = ParsePostTime(result["date"]);
        if (result["imageId"].is_null()) searchedPost.imageId = -1;
        else searchedPost.imageId = result["imageId"];
        
        if (users.find(searchedPost.userId) == users.end()) {
            User user;
            user.username = result["user"]["username"];
            user.userImage = -1;
            users[searchedPost.userId] = user;
        }
        if (!result["tags"].empty())
        {
            for (const auto& tag : result["tags"]) {
                searchedPost.tags.emplace_back(tag);
            }
        }
        int commentCount = 0;
        if (!result["comments"].empty()) {
            for (const auto& comment : result["comments"]) {
                commentCount++;
                Comment newComment;
                
                newComment.id = comment["id"];
                newComment.userId = comment["user"]["id"];
                newComment.text = comment["text"];
				newComment.time = ParsePostTime(comment["date"]);
                searchedPost.comments.emplace_back(newComment);
            }
        }
        searchedPost.allLoaded = false;
		std::thread(&SocialMedia::LoadDependencies, std::ref(searchedPost), -2).detach();
		searchPostOpen = true;
	}
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
            if (dataId == -2) {
				searchedPost.image = HManager::ImageFromRequest(imageData, searchedPost.ratio);
				searchedPost.picLoaded = true;
            }
            else {
				posts[dataId].image = HManager::ImageFromRequest(imageData, posts[dataId].ratio);
				posts[dataId].picLoaded = true;
            }
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
                GLuint idForImage = HManager::ImageFromRequest(imageData, ratioAF);
                imageIndexes[idForImage] = dataId;
                userImages.emplace_back(idForImage);
            } break;
        case 5: {
                images[dataId] = HManager::ImageFromRequest(imageData, postImageRelation[dataId].ratio);
            } break;
        default:
            break;
        }
    }
}

void SocialMedia::LoadProfile(int id)
{
    userImages.clear();
    nlohmann::json jsonData;
    if(id == runtime.id) jsonData = HManager::Request(("images/user/" + std::to_string(runtime.id)).c_str(), "", GET);
	else jsonData = HManager::Request(("images/user/" + std::to_string(id)).c_str(), "", GET);
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
        textureQueue.push(std::make_tuple(std::move(imageData), image, 4));
    }
}

void SocialMedia::MainPage(float& width, float& height)
{

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

    Lss::Top(2 * Lss::VH);
    for (Post& post : posts)
    {
        if (!post.allLoaded) {
            if (post.picLoaded && users[post.userId].pPicLoaded) post.allLoaded = true;
            continue;
        }
        bool needChange = false;
        int validWidth = width * 0.6f;
        std::string id = std::to_string(post.id);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
        Lss::SetColor(ContainerBackground, Background);
        Lss::Child("##" + id, ImVec2(validWidth, post.size), true, Centered | Rounded | Bordering, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        Lss::SetColor(ContainerBackground, ContainerBackground);
        ImGui::PopStyleVar();
        if (post.needChange) {
            startY = ImGui::GetCursorPosY();
            needChange = true;
        }

        Lss::LeftTop(Lss::VW, Lss::VH);
        Lss::Image(users[post.userId].userImage, ImVec2(8 * Lss::VH, 8 * Lss::VH), Rounded | Bordering);

        ImGui::SameLine();
        Lss::Top(2 * Lss::VH);
        Lss::Text(users[post.userId].username, 4 * Lss::VH);

        ImGui::SameLine();
        Lss::SetFontSize(3 * Lss::VH);
        std::string timeOut = CalcTime(post.time);
        float sizeofDay = ImGui::CalcTextSize(timeOut.c_str()).x;
        float availForDay = ImGui::GetContentRegionAvail().x;
        Lss::LeftTop(availForDay - sizeofDay-Lss::VW,Lss::VH);
        Lss::Text(timeOut, 3 * Lss::VH);

        Lss::Left(4 * Lss::VH);
        Lss::Text(post.text, 3 * Lss::VH);

        float good = validWidth * 0.9f;
        ImVec2 faki(good, good * post.ratio);

        if (post.image != -1) {
            Lss::Child("imageFor" + id, faki, true, Bordering | Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::SetCursorPos(ImVec2(0, 0));
            Lss::Image(post.image, faki, Centered);
            Lss::End();
            ImGui::EndChild();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        Lss::LeftTop(3.5f * Lss::VH, 0.5f*Lss::VH);
        float commentMax = validWidth - 7 * Lss::VH;
        float commentSize = 0.0f;
        for (const auto& tags : post.tags)
        {
            Lss::SetFontSize(2.5f * Lss::VH);
            float tagWidth = ImGui::CalcTextSize(("#" + tags).c_str()).x + Lss::VW;
            if (tagWidth + commentSize > commentMax) {
                ImGui::NewLine();
                Lss::Left(3.5f * Lss::VH);
                commentSize = 0.0f;
            }
            commentSize += tagWidth;
            Lss::Child("##" + tags, ImVec2(tagWidth, 3 * Lss::VH), true);
                Lss::Text("#" + tags, 2.5f * Lss::VH, Centered);
                Lss::End();
            ImGui::EndChild();
            ImGui::SameLine();
        }
        ImGui::NewLine();
        ImGui::PopStyleVar();

        Lss::Top(0.5f * Lss::VH);
        Lss::Separator(2);

        static bool likeHovered = false;
        static bool commentHovered = false;

        float widthForButtons = ImGui::GetContentRegionAvail().x;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        bool contains = (runtime.liked.find(post.id) != runtime.liked.end());
        if (post.likeHovered) Lss::SetColor(ContainerBackground, HeavyHighlight);
        else Lss::SetColor(ContainerBackground, Background);
        Lss::Child("##LikeButton"+std::to_string(post.id), ImVec2(widthForButtons / 2, 6 * Lss::VH), false, Trans);
            Lss::Top(Lss::VH);
            if(contains)Lss::Image(likedTexture.GetId(), ImVec2(4 * Lss::VH, 4 * Lss::VH),Centered);
            else Lss::Image(notLikedTexture.GetId(), ImVec2(4 * Lss::VH, 4 * Lss::VH), Centered);
            Lss::Top(Lss::VH);
            Lss::Text(std::to_string(post.likes), 4 * Lss::VH, SameLine);
            Lss::End();
        ImGui::EndChild();
        post.likeHovered = ImGui::IsItemHovered();
        if (ImGui::IsItemClicked()) {
            if (contains) {
                runtime.liked.erase(post.id);
                post.likes--;
            }
            else {
                runtime.liked.insert(post.id);
                post.likes++;
            }
            nlohmann::json jsonData;
            jsonData = HManager::Request(("posts/like/" + std::to_string(post.id)).c_str(), "", POST);
        }

        ImGui::SameLine();

        if (post.commentHovered) Lss::SetColor(ContainerBackground, HeavyHighlight);
        else Lss::SetColor(ContainerBackground, Background);
        Lss::Child("commentButton"+std::to_string(post.id), ImVec2(widthForButtons / 2, 6 * Lss::VH));
            Lss::Top(Lss::VH);
            Lss::Image(commentTexture.GetId(), ImVec2(4 * Lss::VH, 4 * Lss::VH), Centered);
        Lss::End();
        ImGui::EndChild();
        Lss::SetColor(ContainerBackground, ContainerBackground);
        post.commentHovered = ImGui::IsItemHovered();
        if (ImGui::IsItemClicked()) {
            post.openComments = !post.openComments;
            post.needChange = true;
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0.0f, Lss::VH));

        if (post.openComments) {
            static char text[256];
            if (Lss::InputText("commentToSend"+post.id, text, sizeof(text), ImVec2(widthForButtons, 4 * Lss::VH), Centered | Rounded, ImGuiInputTextFlags_EnterReturnsTrue,0, "Share your thoughts"))
            {
                nlohmann::json jsonData;
                jsonData["postId"] = post.id;
                jsonData["text"] = text;
                nlohmann::json result = HManager::Request("comments", jsonData.dump(), POST);
                if (result.is_null()) std::cout << "couldn't send comment\n";
                else {
                    Comment myComment;
                    myComment.time = std::chrono::system_clock::now();
                    myComment.text = text;
                    myComment.id = result["id"];
                    myComment.userId = runtime.id;
                    post.comments.push_back(myComment);
                    post.needChange = true;
                }
            }
        }
        
        if (post.openComments) {
            Lss::Top(Lss::VH);
            if (!post.comments.empty()) {
                Lss::Text("Comments", 3 * Lss::VH);
                ImVec2 commentChildSize;
                if (post.comments.size() == 1) {
                    commentChildSize = ImVec2(ImGui::GetContentRegionAvail().x - 20, 11 * Lss::VH);
                }
                else commentChildSize = ImVec2(ImGui::GetContentRegionAvail().x - 20, 20 * Lss::VH);

                Lss::SetColor(ContainerBackground, Background);
                Lss::Child("CommentsRegion", commentChildSize, false, Centered, ImGuiWindowFlags_NoScrollbar);

                for (auto it = post.comments.rbegin(); it != post.comments.rend(); ++it)
                {
                    Comment& comment = *it;
                    if (!users[comment.userId].pPicLoaded) {
                        if (comment.userId == runtime.id) {
                            users[comment.userId].userImage = runtime.pfpTexture;
                            users[comment.userId].pPicLoaded = true;
                        }
                        continue;
                    }

                    Lss::SetColor(ContainerBackground, LowHighlight);
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

                    std::string name = std::to_string(comment.id);
                    Lss::Child(name, ImVec2(0, 8.5 * Lss::VH));

                    Lss::LeftTop(Lss::VH, Lss::VH);
                    Lss::Image(users[comment.userId].userImage, ImVec2(6 * Lss::VH, 6 * Lss::VH), Rounded);

                    ImGui::SameLine();
                    ImVec2 currentCursor = ImGui::GetCursorPos();

                    Lss::Text(users[comment.userId].username, 3.5f * Lss::VH);

                    ImGui::SameLine();
                    Lss::SetFontSize(2 * Lss::VH);
                    std::string timeOut = CalcTime(comment.time);
                    float sizeofDay = ImGui::CalcTextSize(timeOut.c_str()).x;
                    float availForDay = ImGui::GetContentRegionAvail().x;
                    Lss::LeftTop(availForDay - sizeofDay - Lss::VW, 0.5f*Lss::VH);
                    Lss::Text(timeOut, 2 * Lss::VH);

                    currentCursor.x += Lss::VH;
                    currentCursor.y += 3 * Lss::VH;
                    ImGui::SetCursorPos(currentCursor);
                    Lss::Text(comment.text, 2.5f * Lss::VH);

                    Lss::End();

                    ImGui::EndChild();

                    ImGui::PopStyleVar(1);
                    Lss::SetColor(ContainerBackground, ContainerBackground);

                    Lss::Top(0.25f*Lss::VH);
                    if (post.comments[0].id != comment.id) {
                        Lss::Separator();
                        Lss::Top(0.25f * Lss::VH);
                    }
                }

                Lss::End();

                ImGui::EndChild();
            }
            Lss::SetColor(LowHighlight, LowHighlight);
        }
        if (post.needChange && needChange) {
            endY = ImGui::GetCursorPosY();
            post.size = endY - startY;
            post.needChange = false;
        }

        Lss::End();
        ImGui::EndChild();

        Lss::Top(2 * Lss::VH);
        Lss::End();
    }
    ImGui::EndChild();

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
        viewport->WorkPos.y + viewport->WorkSize.y - 2 * Lss::VH),
        ImGuiCond_Always, ImVec2(0.5f, 1.0f));

	static float modalSize = 0.0f;
	ImVec2 cursorAtCurrent = ImGui::GetCursorPos();
    ImGui::SetNextWindowPos(ImVec2(cursorAtCurrent.x, cursorAtCurrent.y - modalSize / 2));
    if (Lss::Modal("Sup", &openStuff, ImVec2(30 * Lss::VW, modalSize), Centered | Trans | Bordering | Rounded, ImGuiWindowFlags_NoDecoration))
    {
        static bool webCreated = false;
        static bool webWasStarted = false;
        static bool webOpen = false;
        static int prevRows = 0;

        if (created) Explorer::FileExplorerUI(&created);
        else if (wasCreated) {
            std::string imagePath = Explorer::GetImagePath();
            if (imagePath.size() > 2) {
                ImGui::CloseCurrentPopup();
                imageToPostTexture.Init(imagePath);
                imageToPost = imageToPostTexture.GetId();
                wasCreated = false;
            }
            else {
                wasCreated = false;
            }
        }

        if (!webCreated && webOpen) {
            ImGui::CloseCurrentPopup();
            webOpen = false;
        }

        static float webSize = 0.0f;
        if (Lss::Modal("fromWeb", &webCreated, ImVec2(40 * Lss::VW, webSize), Centered | Bordering | Rounded, ImGuiWindowFlags_NoDecoration))
        {
            ImGuiStyle& style = ImGui::GetStyle();
            float originalItemSpacingY = style.ItemSpacing.y;
            style.ItemSpacing.y = style.ItemSpacing.x;

            if (webWasStarted) {
                userImages.clear();
                std::thread(&SocialMedia::LoadProfile, runtime.id).detach();
                webWasStarted = false;
            }

            float webStartPos = ImGui::GetCursorPosY();

            ImVec2 avail = ImGui::GetContentRegionAvail();
            float xWidth = avail.x / 3 - (ImGui::GetStyle().FramePadding.x * 2);
            Lss::Left(ImGui::GetStyle().FramePadding.x);
            int rows = 0;
            if (userImages.size() > 0) rows = 1;
            for (size_t i = 0; i < userImages.size(); i++)
            {
                int width, height;
                glGetTextureLevelParameteriv(userImages[i], 0, GL_TEXTURE_WIDTH, &width);
                glGetTextureLevelParameteriv(userImages[i], 0, GL_TEXTURE_HEIGHT, &height);

                float shiftX = 0.0f;
                float shiftY = 0.0f;
                if (width >= height) {
                    shiftX = (1 - (float)height / width) / 2;
                }
                else {
                    shiftY = (1 - (float)width / height) / 2;
                }
                ImGui::Image(userImages[i], ImVec2(xWidth, xWidth), ImVec2(shiftX, shiftY), ImVec2(1.0f - shiftX, 1.0f - shiftY));
                if (ImGui::IsItemClicked()) {
                    imageToPost = userImages[i];
                    webCreated = false;
                }
                if ((i + 1) % 3 != 0)
                    ImGui::SameLine();
                else {
                    rows++;
                    Lss::Left(ImGui::GetStyle().FramePadding.x);
                }
            }
            if (rows > prevRows) {
                ImGui::CloseCurrentPopup();
                prevRows = rows;
            }

            style.ItemSpacing.y = originalItemSpacingY;

            if (ImGui::IsMouseClicked(0))
            {
                ImVec2 pos = ImGui::GetWindowPos();
                ImVec2 cursorPos = ImGui::GetMousePos();
                ImVec2 size = ImGui::GetWindowSize();
                if (!Lss::InBound(cursorPos, pos, size)) {
                    ImGui::CloseCurrentPopup();
                    webCreated = false;
                }
            }

            float contentHeight = rows * (xWidth + ImGui::GetStyle().ItemSpacing.y);
            webSize = webStartPos + contentHeight + ImGui::GetStyle().ItemSpacing.x;

            Lss::End();
            ImGui::EndPopup();
        }

        ImVec2 addFileButton = ImVec2(12 * Lss::VH, 4 * Lss::VH);

		float startPos = ImGui::GetCursorPosY();
        ImVec2 valid = ImGui::GetContentRegionAvail();
        ImVec2 def = ImGui::GetStyle().FramePadding;
        ImGui::GetStyle().FramePadding = ImVec2(0.0f, 0.0f);


        Lss::SetColor(ContainerBackground, LowHighlight);
        Lss::Child("##Postheader", ImVec2(valid.x, 7 * Lss::VH), false, Rounded);
            Lss::LeftTop(Lss::VW,Lss::VH);
            Lss::Text("Make a post", 5 * Lss::VH);
            Lss::End();
        ImGui::EndChild();
        Lss::SetColor(ContainerBackground, ContainerBackground);
        ImGui::GetStyle().FramePadding = def;

        Lss::Top(0.2 * Lss::VH);
        Lss::Separator(1.0f, 30 * Lss::VW, 4, Centered);

        Lss::LeftTop(Lss::VW, Lss::VH);
        Lss::Text("Share your thoughts", 2 * Lss::VH);
        static char inputtext[128] = "";
        Lss::LeftTop(1.2*Lss::VW, Lss::VH);
        Lss::InputText("Heoooo", inputtext, sizeof(inputtext), ImVec2(26.6 * Lss::VW, 4 * Lss::VH), Rounded, 0, 0, "What is on your mind? :3");


        static std::vector<std::string> tags;
        static std::string textToTags = "None";
        static char tagsText[128] = "";
        static bool focusNextFrame = false;

        Lss::LeftTop(Lss::VW, Lss::VH);
        Lss::Text("Tags", 2 * Lss::VH);
        Lss::LeftTop(1.2 * Lss::VW, Lss::VH);
        if (Lss::InputText("Tagsfortagspost", tagsText, sizeof(tagsText), ImVec2(26.6 * Lss::VW, 4 * Lss::VH), Rounded, ImGuiInputTextFlags_EnterReturnsTrue, 0, "Tags for your post :>"))
        {
            if (tagsText[0] != '\0') {
                if (std::find(tags.begin(), tags.end(), tagsText) == tags.end())
                    tags.emplace_back(tagsText);
                tagsText[0] = '\0';
            }
            focusNextFrame = true;
        }
        if (focusNextFrame)
        {
            ImGui::SetKeyboardFocusHere(-1);
            focusNextFrame = false;
        }

        Lss::LeftTop(2 * Lss::VW, Lss::VH);
        float sizeOfTags = 0.0f;
        for (auto it = tags.begin(); it != tags.end();)
        {
            Lss::SetFontSize(2.5f * Lss::VH);
            float width = ImGui::CalcTextSize(it->c_str()).x+Lss::VW;
            if (sizeOfTags + width > 22 * Lss::VW) {
                sizeOfTags = 0.0f;
                ImGui::NewLine();
                Lss::LeftTop(2 * Lss::VW, Lss::VH);
            }
            sizeOfTags += width;

            if (Lss::Button(("#" + *it).c_str(), ImVec2(width, 3 * Lss::VH), 2.5f * Lss::VH, Rounded))
                it = tags.erase(it);
            else ++it;

            ImGui::SameLine();
        }
        ImGui::NewLine();

        static bool fromDisk = false;
        static bool fromWeb = false;

        Lss::Top(1 * Lss::VH);

        ImGuiStyle& style = ImGui::GetStyle();
        float oldSpacing = style.ItemSpacing.x;

        style.ItemSpacing.x = Lss::VH;

        bool disk = Lss::Button("Disk", ImVec2(valid.x/2 - (ImGui::GetStyle().ItemSpacing.x/2), 4 * Lss::VH), 4 * Lss::VH, (!fromDisk) ? Invisible : None);
        bool web = Lss::Button("Cloud", ImVec2(valid.x / 2 - (ImGui::GetStyle().ItemSpacing.x/2), 4 * Lss::VH), 4 * Lss::VH, SameLine | ((!fromWeb) ? Invisible : None));
        
        style.ItemSpacing.x = oldSpacing;
        if (disk) {
            created = true;
            wasCreated = true;

            fromDisk = true;
            fromWeb = false;
        }
        if (web) {
            webCreated = true;
            webWasStarted = true;
            webOpen = true;
            prevRows = 0;

            fromDisk = false;
            fromWeb = true;
        }

        Lss::Top(Lss::VH);
        Lss::Separator(1.0f, 30 * Lss::VW, 4, Centered);

        if (imageToPost > 0) {
            Lss::Top(2.5f*Lss::VH);
            int width, height;
            glGetTextureLevelParameteriv(imageToPost, 0, GL_TEXTURE_WIDTH, &width);
            glGetTextureLevelParameteriv(imageToPost, 0, GL_TEXTURE_HEIGHT, &height);
            float toPostRatio = (float)height/ width;
            float widthOfImage = 25 * Lss::VW;
            float heightOfImage = widthOfImage * toPostRatio;
            ImVec2 start(0.0f, 0.0f);
            ImVec2 end(1.0f, 1.0f);
            float yPos = ImGui::GetCursorPosY();

            if (heightOfImage > 30*Lss::VH) {
                float toVisualize = 30 * Lss::VH / heightOfImage;
                toVisualize = 1.0f - toVisualize;

                heightOfImage = 30*Lss::VH;

                start.y = toVisualize/4;
                end.y = 1.0f - (toVisualize/4*3);
            }
            Lss::Image(imageToPost, ImVec2(widthOfImage, heightOfImage),Centered, start, end);
            ImVec2 originPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(26.5 * Lss::VW, yPos - Lss::VW));
            if (Lss::Button("X", ImVec2(2*Lss::VW, 2*Lss::VW), 2*Lss::VW)) {
                imageToPost = 0;
            }
            ImGui::SetCursorPos(originPos);
            Lss::Top(Lss::VH);
            Lss::Separator(1.0f, 30 * Lss::VW, 4, Centered);
        }
        Lss::Top(Lss::VH);

        float endPos = ImGui::GetCursorPosY();
        modalSize = endPos - startPos + Lss::VH * 6;

        ImVec2 buttonSize = ImVec2(100, 20);
        ImGui::SetCursorPosY(valid.y - buttonSize.y - 2 * Lss::VH);
        if (Lss::Button("Post##postButton", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered | Rounded))
        {
            std::string imagePath = Explorer::GetImagePath();
            nlohmann::json jsonData;
            
            int imageIndexForPost;
            auto it = imageIndexes.find(imageToPost);
            if (it != imageIndexes.end()) imageIndexForPost = imageIndexes[imageToPost];
            else imageIndexForPost = 0;
            jsonData = HManager::PostRequest(inputtext, imagePath, imageIndexes[imageToPost], tags, (imagePath == "" && imageIndexForPost > 0));
            if (jsonData.is_null()) {
                std::cout << "couldn't post post" << std::endl;
            }
            else {
                posts = {};
                openStuff = false;
                lastId = 0;
                init = true;
                Explorer::ResetPath();
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
}

void SocialMedia::ProfilePage(float& width, float& height, int user) 
{
    static bool imageEditOpen = false;
    static bool openExplorer = false;
    static bool wasOpenExplorer = false;

    static bool imageUploadpen = false;
    static bool openExplorer2 = false;
    static bool wasOpenExplorer2 = false;
    static int selectedImage = -1;
    static bool imageViewOpen = false;
    static bool prevImageViewOpen = false;
    static ImVec2 imageViewSize; 

    static bool thereIsBio = true;

    static std::string fileName;

    if (thereIsBio && users[user].bio.empty()) {
        nlohmann::json result = HManager::Request(("users/" + std::to_string(user)).c_str(), "", GET);
        if (!result["profile_description"].is_null())
            users[user].bio = result["profile_description"];
        else thereIsBio = false;
    }

    if (needImages) {
        std::thread(&SocialMedia::LoadProfile, user).detach();
        needImages = false;
    }

    ImVec2 valid = ImGui::GetContentRegionAvail();
    Lss::Child("Feed", ImVec2(valid.x, 0), false, Centered, ImGuiWindowFlags_NoScrollbar);
        int validWidth = width * 0.6f;
            Lss::Child("##user", ImVec2(validWidth, height), true, Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            if (user == runtime.id) {
                users[user].userImage = runtime.pfpTexture;
                //data change
                if (Lss::Modal("dataChange", &imageEditOpen, ImVec2(30 * Lss::VW, 48 * Lss::VH), Centered | Trans | Rounded | Bordering, ImGuiWindowFlags_NoDecoration))
                {
                    ImVec2 valid = ImGui::GetContentRegionAvail();
                    float halfWidth = valid.x / 2;
                    float halfWidthObj = halfWidth -2.4f * Lss::VW;

                    ImVec2 def = ImGui::GetStyle().FramePadding;
                    ImGui::GetStyle().FramePadding = ImVec2(0.0f, 0.0f);

                    Lss::SetColor(ContainerBackground, LowHighlight);
                    Lss::Child("##Postheader", ImVec2(valid.x, 7 * Lss::VH), false, Rounded);
                    Lss::LeftTop(Lss::VW, Lss::VH);
                    Lss::Text("Change your profile", 5 * Lss::VH);
                    Lss::End();
                    ImGui::EndChild();
                    Lss::SetColor(ContainerBackground, ContainerBackground);
                    ImGui::GetStyle().FramePadding = def;

                    Lss::Top(0.2 * Lss::VH);
                    Lss::Separator(1.0f, 30 * Lss::VW, 4, Centered);

                    Lss::LeftTop(Lss::VW, Lss::VH);

                    float yPos = ImGui::GetCursorPosY();
                    Lss::Text("Profile Image", 2 * Lss::VH);
                    ImGui::SetCursorPos(ImVec2(halfWidth + Lss::VH, yPos));
                    Lss::Text("Username", 2 * Lss::VH);

                    Lss::LeftTop(1.2 * Lss::VW, Lss::VH);
                    yPos = ImGui::GetCursorPosY();

                    if (Lss::Button((fileName.empty()) ? "Select image file" : fileName, ImVec2(halfWidthObj, 4 * Lss::VH), 2 * Lss::VH, Rounded)) {
                        openExplorer = true;
                        wasOpenExplorer = true;
                    }

                    ImGui::SetCursorPos(ImVec2(halfWidth + Lss::VH, yPos));
                    static char usernameText[128] = "";
                    if (usernameText[0] == '\0') std::strcpy(usernameText, runtime.username.c_str());
                    Lss::InputText("usernameInput", usernameText, sizeof(usernameText), ImVec2(halfWidthObj, 4 * Lss::VH), Rounded, 0, 0, "What is on your mind? :3");

                    Lss::LeftTop(Lss::VW, Lss::VH);

                    yPos = ImGui::GetCursorPosY();
                    Lss::Text("Email", 2 * Lss::VH);
                    ImGui::SetCursorPos(ImVec2(halfWidth + Lss::VH, yPos));
                    Lss::Text("Password", 2 * Lss::VH);

                    Lss::LeftTop(1.2 * Lss::VW, Lss::VH);
                    yPos = ImGui::GetCursorPosY();

                    static char emailText[128] = "";
                    if (emailText[0] == '\0') std::strcpy(emailText, runtime.email.c_str());
                    Lss::InputText("emailInput", emailText, sizeof(emailText), ImVec2(halfWidthObj, 4 * Lss::VH), Rounded, 0, 0, "What is on your mind? :3");

                    ImGui::SetCursorPos(ImVec2(halfWidth + Lss::VH, yPos));
                    static char passText[128] = "";
                    if (passText[0] == '\0') std::strcpy(passText, runtime.password.c_str());
                    Lss::InputText("passwordInput", passText, sizeof(passText), ImVec2(halfWidthObj, 4 * Lss::VH), Rounded, 0, 0, "What is on your mind? :3");

                    Lss::LeftTop(Lss::VW, Lss::VH);
;
                    Lss::Text("Description", 2 * Lss::VH);
                    static char detailText[1026] = "";
                    if (detailText[0] == '\0') std::strcpy(detailText, users[user].bio.c_str());
                    Lss::InputText("descriptionInput", detailText, sizeof(detailText), ImVec2(valid.x-2.8f*Lss::VW, 10 * Lss::VH), Centered | MultiLine | Rounded);


                    if (openExplorer) Explorer::FileExplorerUI(&openExplorer);
                    else if (wasOpenExplorer) {
                        std::string imagePath = Explorer::GetImagePath();
                        if (imagePath.size() > 2) {
                            userImageTexture.Init(imagePath);
                            wasOpenExplorer = false;
                        }
                        else {
                            wasOpenExplorer = false;
                        }
                    }

                    if (userImageTexture.GetId() > 0)
                    {
                        std::string imagePath = Explorer::GetImagePath();
                        fileName = imagePath.substr(imagePath.find_last_of('\\') + 1);
                    }

                    ImVec2 buttonSize = ImVec2(100, 20);
                    ImGui::SetCursorPosY(valid.y - buttonSize.y - 2 * Lss::VH);
                    if (Lss::Button("Modify##modifyImage", ImVec2(valid.x, 4 * Lss::VH), 3 * Lss::VH, Centered | Rounded))
                    {
                        std::string imagePath = Explorer::GetImagePath();
                        if (userImageTexture.GetId() > 0) {
                            nlohmann::json jsonData = HManager::ImageUploadRequest(imagePath, 1);
                            if (!jsonData.is_null()) {
                                imageEditOpen = false;
                                runtime.pfpTexture = userImageTexture.GetId();
                            }
                            Explorer::ResetPath();
                        }
                        nlohmann::json userPatch;
                        if (runtime.username != usernameText) userPatch["username"] = usernameText;
                        if (runtime.email != emailText) userPatch["email"] = emailText;
                        if (runtime.password != passText) userPatch["password"] = passText;
                        if (users[user].bio != detailText) userPatch["profile_description"] = detailText;
                        if (!userPatch.empty()) {
                            HManager::Request("users", userPatch.dump(), PATCH);
                            if (!userPatch.is_null()) {
                                std::cout << "patched" << std::endl;
                                if (runtime.username != usernameText) runtime.username = usernameText;
                                if (runtime.email != emailText) runtime.email = emailText;
                                if (runtime.password != passText) runtime.password = passText;
                                if (users[user].bio != detailText) users[user].bio = detailText;

                                std::vector<uint8_t> imageData = HManager::ImageRequest(("users/" + std::to_string(runtime.id) + "/pfp").c_str());
                                auto* textureQueue = SocialMedia::GetTextureQueue();
                                textureQueue->push(std::make_tuple(imageData, 0, 3));
                            }
                        }
                        else {
                            std::cout << "did not sent" << std::endl;
                        }
                    }


                    if (!openExplorer && ImGui::IsMouseClicked(0))
                    {
                        ImVec2 pos = ImGui::GetWindowPos();
                        ImVec2 cursorPos = ImGui::GetMousePos();
                        ImVec2 size = ImGui::GetWindowSize();
                        if (!Lss::InBound(cursorPos, pos, size)) {
                            imageEditOpen = false;

                            usernameText[0] = '\0';
                            emailText[0] = '\0';
                            passText[0] = '\0';
                            detailText[0] = '\0';

                            ImGui::CloseCurrentPopup();
                        }
                    }
                    Lss::End();
                    ImGui::EndPopup();
                }

                static float imageAddYPrev = 0.0f;
                static float imageAddY = 25*Lss::VH;
                //image add
                if (Lss::Modal("imageToGalery", &imageUploadpen, ImVec2(30 * Lss::VW, imageAddY), Centered | Trans | Rounded | Bordering, ImGuiWindowFlags_NoDecoration))
                {
                    static bool privating = true;
                    float startPos = ImGui::GetCursorPosY();
                    ImVec2 valid = ImGui::GetContentRegionAvail();

                    ImVec2 def = ImGui::GetStyle().FramePadding;
                    ImGui::GetStyle().FramePadding = ImVec2(0.0f, 0.0f);

                    Lss::SetColor(ContainerBackground, LowHighlight);
                    Lss::Child("##Postheader", ImVec2(valid.x, 7 * Lss::VH), false, Rounded);
                    Lss::LeftTop(Lss::VW, Lss::VH);
                    Lss::Text("Add file to galery", 5 * Lss::VH);
                    Lss::End();
                    ImGui::EndChild();
                    Lss::SetColor(ContainerBackground, ContainerBackground);
                    ImGui::GetStyle().FramePadding = def;

                    Lss::Top(0.2 * Lss::VH);
                    Lss::Separator(2.0f, 30 * Lss::VW, 4, Centered);
                    Lss::Top(Lss::VH);

                    Lss::Left(0.5f * Lss::VW);
                    ImVec2 addFileButton = ImVec2(valid.x/2-Lss::VW, 5 * Lss::VH);
                    if (Lss::Button("Add File", addFileButton, 4 * Lss::VH, Rounded)) {
                        openExplorer2 = true;
                        wasOpenExplorer2 = true;
                    }

                    ImGui::SameLine();
                    Lss::Left(Lss::VW);
                    Lss::Child("selectPrivacy", ImVec2(valid.x/2-Lss::VW, 5 * Lss::VH), true, Bordering, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                        ImVec2 validPrivacy = ImGui::GetWindowSize();
                        ImGui::SetCursorPos(ImVec2(0, 0));
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                        if (privating) Lss::SetColor(LowHighlight, HeavyHighlight);
                        if (Lss::Button("Private", ImVec2(validPrivacy.x / 2, 5 * Lss::VH), 4 * Lss::VH))
                            privating = true;
                        if (privating) Lss::SetColor(LowHighlight, LowHighlight);

                        if (!privating) Lss::SetColor(LowHighlight, HeavyHighlight);
                        if (Lss::Button("Public", ImVec2(validPrivacy.x / 2, 5 * Lss::VH), 4 * Lss::VH, SameLine))
                            privating = false;
                        if (!privating) Lss::SetColor(LowHighlight, LowHighlight);

                        ImGui::PopStyleVar();
                        Lss::End();
                    ImGui::EndChild();

                    if (openExplorer2) Explorer::FileExplorerUI(&openExplorer2);
                    else if (wasOpenExplorer2) {
                        std::string imagePath = Explorer::GetImagePath();
                        if (imagePath.size() > 2) {
                            userImageTexture.Init(imagePath);
                        }
                        wasOpenExplorer2 = false;
                    }

                    Lss::Top(Lss::VH);
                    Lss::Separator(1.0f, 30 * Lss::VW, 4, Centered);

                    if (userImageTexture.GetId() > 0) {
                        Lss::Top(2.5f * Lss::VH);
                        int width, height;
                        glGetTextureLevelParameteriv(userImageTexture.GetId(), 0, GL_TEXTURE_WIDTH, &width);
                        glGetTextureLevelParameteriv(userImageTexture.GetId(), 0, GL_TEXTURE_HEIGHT, &height);
                        float toPostRatio = (float)height / width;
                        float widthOfImage = 25 * Lss::VW;
                        float heightOfImage = widthOfImage * toPostRatio;
                        ImVec2 start(0.0f, 0.0f);
                        ImVec2 end(1.0f, 1.0f);
                        float yPos = ImGui::GetCursorPosY();

                        if (heightOfImage > 30 * Lss::VH) {
                            float toVisualize = 30 * Lss::VH / heightOfImage;
                            toVisualize = 1.0f - toVisualize;

                            heightOfImage = 30 * Lss::VH;

                            start.y = toVisualize / 4;
                            end.y = 1.0f - (toVisualize / 4 * 3);
                        }
                        Lss::Image(userImageTexture.GetId(), ImVec2(widthOfImage, heightOfImage), Centered, start, end);
                        ImVec2 originPos = ImGui::GetCursorPos();
                        ImGui::SetCursorPos(ImVec2(26.5 * Lss::VW, yPos - Lss::VW));
                        if (Lss::Button("X", ImVec2(2 * Lss::VW, 2 * Lss::VW), 2 * Lss::VW)) {
                            MyTexture heo;
                            userImageTexture = heo;
                        }
                        ImGui::SetCursorPos(originPos);
                        Lss::Top(Lss::VH);
                        Lss::Separator(1.0f, 30 * Lss::VW, 4, Centered);
                    }
                    Lss::Top(Lss::VH);
                    float endPos = ImGui::GetCursorPosY();
                    
                    ImVec2 buttonSize = ImVec2(valid.x, 4 * Lss::VH);
                    ImGui::SetCursorPosY(valid.y - buttonSize.y);
                    if (Lss::Button("Add##addImage", buttonSize, 3 * Lss::VH, Centered | Rounded))
                    {
                        std::string imagePath = Explorer::GetImagePath();
                        if (imagePath.size() > 2 && userImageTexture.GetId() > 0) {
                            Explorer::ResetPath();
                            std::string piranah;
                            if (privating) piranah = "private";
                            else piranah = "public";
                            nlohmann::json jsonData = HManager::ImageUploadRequest(imagePath, 0, piranah);
                            if (!jsonData.is_null()) {
                                imageUploadpen = false;
                                needImages = true;
                            }
                        }
                    }

                    imageAddY = endPos - startPos + buttonSize.y + 2*Lss::VH;
                    if (imageAddY != imageAddYPrev) {
                        imageAddYPrev = imageAddY;
                        ImGui::CloseCurrentPopup();
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
            }

            Lss::Image(users[user].userImage, ImVec2(20 * Lss::VH, 20 * Lss::VH), Rounded);
            if (ImGui::IsItemClicked()) {
                imageEditOpen = true;
            }

            ImGui::SameLine();
            Lss::Top(7.5 * Lss::VH);
            if(runtime.id == user) Lss::Text(runtime.username, 5 * Lss::VH);
            else Lss::Text(users[user].username, 5 * Lss::VH);


            if (user == runtime.id) {
                ImGui::SameLine();
                float availForButton = ImGui::GetContentRegionAvail().x;
                Lss::SetFontSize(4 * Lss::VH);
                float buttonSize = ImGui::CalcTextSize("Add image").x+ 3*Lss::VW;
                Lss::LeftTop(availForButton-buttonSize, 7.5 * Lss::VH);
                if (Lss::Button("Add image", ImVec2(20 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH,Rounded))
                {
                    imageUploadpen = true;
                }
            }
            Lss::LeftTop(2*Lss::VW,2*Lss::VH);
            Lss::Text(users[user].bio, 2 * Lss::VH);

            Lss::Top(2 * Lss::VH);
            Lss::Separator(2.0f);

            ImVec2 avail = ImGui::GetContentRegionAvail();
            float xWidth = avail.x / 3;
            for (size_t i = 0; i < userImages.size(); i++)
            {
                int width, height;
                glGetTextureLevelParameteriv(userImages[i], 0, GL_TEXTURE_WIDTH, &width);
                glGetTextureLevelParameteriv(userImages[i], 0, GL_TEXTURE_HEIGHT, &height);

                float shiftX = 0.0f;
                float shiftY = 0.0f;
                if (width >= height) {
                    shiftX = (1 - (float)height / width) / 2;
                }
                else {
                    shiftY = (1 - (float)width / height) / 2;
                }
                ImGui::Image(userImages[i], ImVec2(xWidth, xWidth),ImVec2(shiftX, shiftY),ImVec2(1.0f-shiftX, 1.0f-shiftY));

                if (ImGui::IsItemClicked()) {
                    imageViewOpen = true;
                    selectedImage = userImages[i];
                    float max = 50 * Lss::VW;
                    if (width > max) {
                        float ratio = max / width;
                        width = max;
                        height *= ratio;
                    }
                    if (height > max) {
                        float ratio = max / height;
                        height = max;
                        width *= ratio;
                    }
                    imageViewSize = ImVec2(width, height);
                }

                if ((i + 1) % 3 != 0)
                    ImGui::SameLine();
                else {
                    Lss::Top(0.3f * Lss::VH);
                }
            }

            if (Lss::Modal("imageViewModal", &imageViewOpen, imageViewSize, Centered | Trans, ImGuiWindowFlags_NoDecoration))
            {
                ImVec2 valid = ImGui::GetContentRegionAvail();
                Lss::Image(selectedImage, valid);

                if (prevImageViewOpen && ImGui::IsMouseClicked(0))
                {
                    ImVec2 pos = ImGui::GetWindowPos();
                    ImVec2 cursorPos = ImGui::GetMousePos();
                    ImVec2 size = ImGui::GetWindowSize();
                    if (!Lss::InBound(cursorPos, pos, size)) {
                        imageViewOpen = false;
                        selectedImage = -1;
                        prevImageViewOpen = false;
                        ImGui::CloseCurrentPopup();
                    }
                }

                if (imageViewOpen) prevImageViewOpen = true;

                Lss::End();
                ImGui::EndPopup();
            }


            Lss::End();
        ImGui::EndChild();
    ImGui::EndChild();
}

void SocialMedia::SettingsPage()
{
    ImVec2 valid = ImGui::GetContentRegionAvail();
    Lss::Child("Feed", ImVec2(valid.x, 0), false, Centered, ImGuiWindowFlags_NoScrollbar);

        Lss::Text("Profile settings", 5 * Lss::VH);

        ImGui::SameLine();
        Lss::Top(2.4f * Lss::VH);
        Lss::Separator(2.0f, 0.0f, 4);


        Lss::Left(5 * Lss::VW);
        Lss::Child("##UserVariables", ImVec2(0, 10 * Lss::VH), false, Centered, ImGuiWindowFlags_NoScrollbar);

            Lss::Text("Username: ", 3 * Lss::VH);

            ImGui::SameLine();
            Lss::Top(0.25f * Lss::VH);
            static char usernameText[128];
            if (usernameText[0] == '\0') std::strcpy(usernameText, runtime.username.c_str());
            if (Lss::InputText("usernameNono", usernameText, sizeof(usernameText), ImVec2(25 * Lss::VH, 2.5f * Lss::VH), Rounded, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                runtime.username = usernameText;
                nlohmann::json body;
                body["username"] = usernameText;
                HManager::Request("users", body.dump(), PATCH);
            }

            Lss::Text("Password: ", 3 * Lss::VH);

            ImGui::SameLine();
            Lss::Top(0.25f * Lss::VH);
            static char passwordText[128];
            if (passwordText[0] == '\0') std::strcpy(passwordText, runtime.password.c_str());
            if (Lss::InputText("passwordNono", passwordText, sizeof(passwordText), ImVec2(25 * Lss::VH, 2.5f * Lss::VH), Rounded, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (passwordText[0] != '\0')
                {
                    runtime.password = passwordText;
                    nlohmann::json body;
                    body["password"] = passwordText;
                    HManager::Request("users", body.dump(), PATCH);
                }
            }

            Lss::Text("Email: ", 3 * Lss::VH);

            ImGui::SameLine();
            Lss::Top(0.25f * Lss::VH);
            static char emailText[128];
            if (emailText[0] == '\0') std::strcpy(emailText, runtime.email.c_str());
            if (Lss::InputText("emailNono", emailText, sizeof(emailText), ImVec2(25 * Lss::VH, 2.5f * Lss::VH), Rounded, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                runtime.email = emailText;
                nlohmann::json body;
                body["email"] = emailText;
                HManager::Request("users", body.dump(), PATCH);
            }

            Lss::End();

        ImGui::EndChild();

        Lss::Text("Network Variables", 5 * Lss::VH);

        ImGui::SameLine();
        Lss::Top(2.4f * Lss::VH);
        Lss::Separator(2.0f, 0.0f, 4);


        Lss::Left(5 * Lss::VW);
        Lss::Child("##NetworkVariables", ImVec2(0, 4.5 * Lss::VH), false, Centered, ImGuiWindowFlags_NoScrollbar);

            Lss::Text("Server IP: ", 3 * Lss::VH);

            ImGui::SameLine();
            Lss::Top(0.25f * Lss::VH);
            static char ipText[128] = "";
            if (ipText[0] == '\0') std::strcpy(ipText, runtime.ip.c_str());
            if (Lss::InputText("faku", ipText, sizeof(ipText), ImVec2(25 * Lss::VH, 2.5f * Lss::VH), Rounded, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                runtime.ip = ipText;
                std::thread(&HManager::InitUser).detach();
            }

            Lss::End();

        ImGui::EndChild();


        Lss::Text("Drawing Variables", 5 * Lss::VH);

        ImGui::SameLine();
        Lss::Top(2.4f * Lss::VH);
        Lss::Separator(2.0f, 0.0f, 4);


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
}

void SocialMedia::SearchPage(float& width, float& height)
{
    if (searchMode == 0) {

        static int searchRequestMode = 1;

        static bool search = true;
        static int count = 0;
        static bool searching = true;

        static bool searchingUsers = false;
        static bool searchingPost = false;


        ImVec2 valid = ImGui::GetContentRegionAvail();
        Lss::Child("Feed", ImVec2(valid.x, 0), false, Centered, ImGuiWindowFlags_NoScrollbar);
        static char searchText[128] = "";
        bool hihi = Lss::InputText("faku", searchText, sizeof(searchText), ImVec2(50 * Lss::VH, 5.0f * Lss::VH), Rounded | Centered, ImGuiInputTextFlags_EnterReturnsTrue);
        if (hihi) searchRequestMode = 1;
        ImGui::SameLine();
        Lss::Left(3*Lss::VH);

        if (Lss::Button("SR", ImVec2(5 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH)) {
            searchRequestMode = 1;
            hihi = true;
        }
        if (Lss::Button("PF", ImVec2(5 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, SameLine))
        {
            searchRequestMode = 2;
            std::cout << "mode: " << searchRequestMode << std::endl;
            hihi = true;

        }

        if (hihi) {
            searching = true;
            searchedUser.clear();
            images.clear();
            postImageRelation.clear();
        }

        if (search) {
            Lss::Top(10 * Lss::VH);
            std::string searchtext = "Searching";
            for (size_t i = 100; i < count; i += 100)searchtext += ".";
            Lss::Text(searchtext, 4 * Lss::VH, Centered);
            count++;
            if (count >= 401) {
                count = 0;
            }
            if (images.size() > 0 || searchedUser.size() > 0) search = false;
        }

        Lss::Top(Lss::VH);
        Lss::Separator(2.0f, 0.0f, 4);
        Lss::Top(Lss::VH);

        if (images.size() <= 0 && searchedUser.size() <= 0 && searching) {
            std::cout << "mode: " << searchRequestMode << std::endl;
            std::thread(&SocialMedia::SearchStuff, searchText, searchRequestMode).detach();
            searching = false;
        }
        if (searchedUser.size() > 0)
        {
            int validWidth = width * 0.6f;
            Lss::Top(Lss::VH);
            height = searchedUser.size() * 8 * Lss::VH;
            if (height > 18 * Lss::VH) height = 18 * Lss::VH;
            Lss::Child("##UserSearchResults", ImVec2(validWidth, height), false, Centered, ImGuiWindowFlags_NoScrollbar);
            for (const auto& user : searchedUser)
            {
				Lss::Child("##" + users[user].username, ImVec2(validWidth, 8 * Lss::VH), false, Centered);
                    Lss::Image(users[user].userImage, ImVec2(8 * Lss::VH, 8 * Lss::VH), Rounded);
                    ImGui::SameLine();
                    Lss::Top(2 * Lss::VH);
                    Lss::Text(users[user].username, 4 * Lss::VH);
                    Lss::End();
				ImGui::EndChild();
                if (ImGui::IsItemClicked()) {
                    userImages.clear();
                    needImages = true;
					searchMode = 1;
                    selectedUser = user;
                }
            }
            Lss::End();
            ImGui::EndChild();
        }
        if (images.size() > 0)
        {
            std::array<float, 3> yPos = { 0.0f, 0.0f, 0.0f };
            std::array<float, 3> ySize = { 0.0f, 0.0f, 0.0f };
            ImVec2 mod = ImGui::GetStyle().FramePadding;
            int validWidth = width * 0.6f;
            static float heightOfImages = height - 7.0f * Lss::VH;
            Lss::Child("##SearchResults", ImVec2(validWidth, heightOfImages), false, Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


            float xWidth = validWidth / 3;
            int i = 0;
            for (const auto postImage : postImageRelation)
            {
                ImGui::SetCursorPosY(yPos[i]);
                ImGui::SetCursorPosX(xWidth * i + mod.x * i);
                ImGui::Image(images[postImage.first], ImVec2(xWidth - mod.x, xWidth * postImage.second.ratio));
                if (ImGui::IsItemClicked()) {
                    GetPostForSearch(postImage.first);
                }
                float nextYPos = ImGui::GetCursorPosY() + mod.y;
                ySize[i] += nextYPos - yPos[i];
                yPos[i] = nextYPos;

                if (i == 2) i = 0;
                else i++;
            }

            for (size_t i = 0; i < 3; i++)
            {
                if (ySize[i] > heightOfImages) heightOfImages = ySize[i];
            }

            ImGui::EndChild();
        }
        if (searchPostOpen) {
            static int postWidth = width * 0.5f;
            static int postHeight = 40 * Lss::VH;
            
            static bool canClick = false;

            static float prevSize = 0.0f;

            static ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImVec2 pos = ImVec2(center.x - postWidth * 0.5f, center.y - postHeight * 0.5f);
            ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
            ImVec4 currentBgColor = ImGui::GetStyle().Colors[ImGuiCol_PopupBg];
            ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = ImVec4(0.0902f, 0.0902f, 0.2471f, 1.0f);

            if (!searchedPost.allLoaded) {
                if (searchedPost.picLoaded && users[searchedPost.userId].pPicLoaded) {
                    searchedPost.allLoaded = true;
                }
            }

            if (Lss::Modal("searchedPost", &searchPostOpen, ImVec2(postWidth, prevSize), Bordering | Rounded | Centered, ImGuiWindowFlags_NoDecoration))
            {
                float startPos = ImGui::GetCursorPosY();

                ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.6f);
                float validWidth = ImGui::GetContentRegionAvail().x;
                if (!searchedPost.allLoaded) {
                    if (searchedPost.picLoaded && users[searchedPost.userId].pPicLoaded) {
                        searchedPost.allLoaded = true;
                        ImGui::CloseCurrentPopup();
                    }
                }

                Lss::LeftTop(Lss::VW, Lss::VH);
                Lss::Image(users[searchedPost.userId].userImage, ImVec2(8 * Lss::VH, 8 * Lss::VH), Rounded | Bordering);

                ImGui::SameLine();
                Lss::Top(2 * Lss::VH);
                Lss::Text(users[searchedPost.userId].username, 4 * Lss::VH);

                ImGui::SameLine();
                Lss::SetFontSize(3 * Lss::VH);
                std::string timeOut = CalcTime(searchedPost.time);
                float sizeofDay = ImGui::CalcTextSize(timeOut.c_str()).x;
                float availForDay = ImGui::GetContentRegionAvail().x;
                Lss::LeftTop(availForDay - sizeofDay - Lss::VW, Lss::VH);
                Lss::Text(timeOut, 3 * Lss::VH);

                ImGui::PushTextWrapPos();
                Lss::Left(4 * Lss::VH);
                Lss::Text(searchedPost.text, 3 * Lss::VH);
                ImGui::PopTextWrapPos();

                float good = validWidth * 0.9f;
                float goodY = good * searchedPost.ratio;
                if (goodY > 50 * Lss::VH) {
                    good = good * (50 * Lss::VH / goodY);
                    goodY = 50 * Lss::VH;
                }
                ImVec2 faki(good, goodY);
                
                if (searchedPost.image != -1) {
                    Lss::Child("imageForIDK", faki, true, Bordering | Centered, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                    ImGui::SetCursorPos(ImVec2(0, 0));
                    Lss::Image(searchedPost.image, faki, Centered);
                    Lss::End();
                    ImGui::EndChild();
                }

                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
                Lss::LeftTop(3.5f * Lss::VH, 0.5f * Lss::VH);
                float commentMax = validWidth - 7 * Lss::VH;
                float commentSize = 0.0f;
                for (const auto& tags : searchedPost.tags)
                {
                    Lss::SetFontSize(2.5f * Lss::VH);
                    float tagWidth = ImGui::CalcTextSize(("#" + tags).c_str()).x + Lss::VW;
                    if (tagWidth + commentSize > commentMax) {
                        ImGui::NewLine();
                        Lss::Left(3.5f * Lss::VH);
                        commentSize = 0.0f;
                    }
                    commentSize += tagWidth;
                    Lss::Child("##somethinForNoCrash" + tags, ImVec2(tagWidth, 3 * Lss::VH), true);
                    Lss::Text("#" + tags, 2.5f * Lss::VH, Centered);
                    Lss::End();
                    ImGui::EndChild();
                    ImGui::SameLine();
                }
                ImGui::NewLine();
                ImGui::PopStyleVar();

                Lss::Top(0.5f * Lss::VH);
                Lss::Separator(2);

                if (canClick && ImGui::IsMouseClicked(0))
                {
                    ImVec2 pos = ImGui::GetWindowPos();
                    ImVec2 cursorPos = ImGui::GetMousePos();
                    ImVec2 size = ImGui::GetWindowSize();
                    if (!Lss::InBound(cursorPos, pos, size)) {
                        Post tmpPost;
                        searchedPost = tmpPost;
                        ImGui::CloseCurrentPopup();
                        searchPostOpen = false;
                    }
                }

                static bool likeHovered = false;
                static bool commentHovered = false;

                float widthForButtons = ImGui::GetContentRegionAvail().x;

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                bool contains = (runtime.liked.find(searchedPost.id) != runtime.liked.end());
                if (searchedPost.likeHovered) Lss::SetColor(ContainerBackground, HeavyHighlight);
                else Lss::SetColor(ContainerBackground, Background);
                Lss::Child("##LikeButton" + std::to_string(searchedPost.id), ImVec2(widthForButtons / 2, 6 * Lss::VH), false, Trans);
                Lss::Top(Lss::VH);
                if (contains)Lss::Image(likedTexture.GetId(), ImVec2(4 * Lss::VH, 4 * Lss::VH), Centered);
                else Lss::Image(notLikedTexture.GetId(), ImVec2(4 * Lss::VH, 4 * Lss::VH), Centered);
                Lss::Top(Lss::VH);
                Lss::Text(std::to_string(searchedPost.likes), 4 * Lss::VH, SameLine);
                Lss::End();
                ImGui::EndChild();
                searchedPost.likeHovered = ImGui::IsItemHovered();
                if (ImGui::IsItemClicked()) {
                    if (contains) {
                        runtime.liked.erase(searchedPost.id);
                        searchedPost.likes--;
                    }
                    else {
                        runtime.liked.insert(searchedPost.id);
                        searchedPost.likes++;
                    }
                    nlohmann::json jsonData;
                    jsonData = HManager::Request(("posts/like/" + std::to_string(searchedPost.id)).c_str(), "", POST);
                    if (jsonData.is_null()) {
                        std::cout << "couldn't post like" << std::endl;
                    }
                }

                ImGui::SameLine();

                if (searchedPost.commentHovered) Lss::SetColor(ContainerBackground, HeavyHighlight);
                else Lss::SetColor(ContainerBackground, Background);
                Lss::Child("commentButton" + std::to_string(searchedPost.id), ImVec2(widthForButtons / 2, 6 * Lss::VH));
                Lss::Top(Lss::VH);
                Lss::Image(commentTexture.GetId(), ImVec2(4 * Lss::VH, 4 * Lss::VH), Centered);
                Lss::End();
                ImGui::EndChild();
                Lss::SetColor(ContainerBackground, ContainerBackground);
                searchedPost.commentHovered = ImGui::IsItemHovered();
                if (ImGui::IsItemClicked()) {
                    searchedPost.openComments = !searchedPost.openComments;
                    searchedPost.needChange = true;
                }
                ImGui::PopStyleVar();
                ImGui::Dummy(ImVec2(0.0f, Lss::VH));

                if (searchedPost.openComments) {
                    static char text[256];
                    if (Lss::InputText("commentToSend" + searchedPost.id, text, sizeof(text), ImVec2(widthForButtons, 4 * Lss::VH), Centered | Rounded, ImGuiInputTextFlags_EnterReturnsTrue, 0, "Share your thoughts"))
                    {
                        nlohmann::json jsonData;
                        jsonData["postId"] = searchedPost.id;
                        jsonData["text"] = text;
                        nlohmann::json result = HManager::Request("comments", jsonData.dump(), POST);
                        if (result.is_null()) std::cout << "couldn't send comment\n";
                        else {
                            Comment myComment;
                            myComment.time = std::chrono::system_clock::now();
                            myComment.text = text;
                            myComment.id = result["id"];
                            myComment.userId = runtime.id;
                            searchedPost.comments.push_back(myComment);
                            searchedPost.needChange = true;
                        }
                    }
                }

                if (searchedPost.openComments) {
                    Lss::Top(Lss::VH);
                    if (!searchedPost.comments.empty()) {
                        Lss::Text("Comments", 3 * Lss::VH);
                        ImVec2 commentChildSize;
                        if (searchedPost.comments.size() == 1) {
                            commentChildSize = ImVec2(ImGui::GetContentRegionAvail().x - 20, 11 * Lss::VH);
                        }
                        else commentChildSize = ImVec2(ImGui::GetContentRegionAvail().x - 20, 20 * Lss::VH);

                        Lss::SetColor(ContainerBackground, Background);
                        Lss::Child("CommentsRegion", commentChildSize, false, Centered, ImGuiWindowFlags_NoScrollbar);

                        for (auto it = searchedPost.comments.rbegin(); it != searchedPost.comments.rend(); ++it)
                        {
                            Comment& comment = *it;
                            if (!users[comment.userId].pPicLoaded) continue;

                            Lss::SetColor(ContainerBackground, LowHighlight);
                            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

                            std::string name = std::to_string(comment.id);
                            Lss::Child(name, ImVec2(0, 8.5 * Lss::VH));

                            Lss::LeftTop(Lss::VH, Lss::VH);
                            Lss::Image(users[comment.userId].userImage, ImVec2(6 * Lss::VH, 6 * Lss::VH), Rounded);

                            ImGui::SameLine();
                            ImVec2 currentCursor = ImGui::GetCursorPos();

                            Lss::Text(users[comment.userId].username, 3.5f * Lss::VH);

                            ImGui::SameLine();
                            Lss::SetFontSize(2 * Lss::VH);
                            std::string timeOut = CalcTime(comment.time);
                            float sizeofDay = ImGui::CalcTextSize(timeOut.c_str()).x;
                            float availForDay = ImGui::GetContentRegionAvail().x;
                            Lss::LeftTop(availForDay - sizeofDay - Lss::VW, 0.5f * Lss::VH);
                            Lss::Text(timeOut, 2 * Lss::VH);

                            currentCursor.x += Lss::VH;
                            currentCursor.y += 3 * Lss::VH;
                            ImGui::SetCursorPos(currentCursor);
                            Lss::Text(comment.text, 2.5f * Lss::VH);

                            Lss::End();

                            ImGui::EndChild();

                            ImGui::PopStyleVar(1);
                            Lss::SetColor(ContainerBackground, ContainerBackground);

                            Lss::Top(0.25f * Lss::VH);
                            if (searchedPost.comments[0].id != comment.id) {
                                Lss::Separator();
                                Lss::Top(0.25f * Lss::VH);
                            }
                        }

                        Lss::End();

                        ImGui::EndChild();
                    }
                    Lss::SetColor(LowHighlight, LowHighlight);
                }

                float endPos = ImGui::GetCursorPosY();

                if (endPos - startPos != prevSize) {
                    prevSize = endPos - startPos;
                    searchedPost.needChange = true;
                }


                if (searchedPost.needChange) {
                    ImGui::CloseCurrentPopup();
                    searchedPost.needChange = false;
                }

                canClick = true;

                Lss::End();
                ImGui::EndPopup();
            }
            ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = currentBgColor;
        }
        Lss::End();
        ImGui::EndChild();
    } else {
        ProfilePage(width, height, selectedUser);
    }
}

void RoomsRequest()
{
    nlohmann::json result = HManager::Request("rooms", "", GET);
    if (result.is_null()) std::cerr << "Can't get rooms\n";
    for (auto& item : result) {
        Room roome;
        roome.roomName = item["name"];
        roome.ownerName = item["owner"]["username"];
        roome.userCount = item["playerCount"];
        roome.capacity = item["maxPlayers"];
        roome.password = item["passwordRequired"];
        rooms.emplace_back(roome);
    }
}

void SocialMedia::RoomPage(float& width, float& height)
{
    if (needRooms) {
        std::thread(&RoomsRequest).detach();
        needRooms = false;
    }
    static bool openJoin = false;
    static bool openCreate = false;
    static bool privateLobby = false;
    static std::string roomName = "";
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImVec2 valid = ImGui::GetContentRegionAvail();
    static char searchRoomText[256];
    Lss::Left(0.05f * valid.x);
    Lss::InputText("searchRoom", searchRoomText, sizeof(searchRoomText), ImVec2(valid.x * 0.7f, 5 * Lss::VH), Rounded);
    ImGui::SameLine();
    Lss::Left(0.05f * valid.x);
    if (Lss::Button("Create", ImVec2(valid.x * 0.2f, 5 * Lss::VH), 4 * Lss::VH))
    {
        openCreate = true;
    }

    if (Lss::Modal("Sup", &openCreate, ImVec2(20 * Lss::VW, 40 * Lss::VH), Centered | Rounded | Bordering, ImGuiWindowFlags_NoDecoration))
    {
        ImVec2 validSup = ImGui::GetContentRegionAvail();

        ImVec2 def = ImGui::GetStyle().FramePadding;
        ImGui::GetStyle().FramePadding = ImVec2(0.0f, 0.0f);

        Lss::SetColor(ContainerBackground, LowHighlight);
        Lss::Child("##Lobbyheader", ImVec2(validSup.x, 7 * Lss::VH), false, Rounded);
        Lss::LeftTop(Lss::VW, Lss::VH);
        Lss::Text("Create a room", 5 * Lss::VH);
        Lss::End();
        ImGui::EndChild();
        Lss::SetColor(ContainerBackground, ContainerBackground);
        ImGui::GetStyle().FramePadding = def;

        ImGui::SetCursorPosX(0);
        Lss::Top(Lss::VH);
        Lss::Separator(1.0f, 20 * Lss::VW, 4);

        Lss::Top(2*Lss::VH);
        Lss::Text("Room", 2 * Lss::VH);
        Lss::Separator();
        Lss::Top(0.2f * Lss::VH);

        static char nameText[128] = "";
        static char passwordText[128] = "";

        Lss::SetFontSize(2.5f*Lss::VH);
        float roomSize = ImGui::CalcTextSize("Room name: ").x;
        float passSize = ImGui::CalcTextSize("Password: ").x;

        Lss::Left(Lss::VH);
        Lss::Text("Room name: ", 2.5f * Lss::VH);
        ImGui::SameLine();
        Lss::InputText("roomName", nameText, sizeof(nameText), ImVec2(10.75f * Lss::VW, 3 * Lss::VH), Rounded, 0, 0, "The room's name");

        Lss::Left(Lss::VH);
        Lss::Text("Password: ", 2.5f * Lss::VH);
        ImGui::SameLine();
        Lss::Left(roomSize - passSize);
        Lss::InputText("roomPassword", passwordText, sizeof(passwordText), ImVec2(10.75f * Lss::VW, 3 * Lss::VH), Rounded, 0, 0, "The room's password");

        ImGui::NewLine();

        Lss::Text("Canvas", 2 * Lss::VH);
        Lss::Separator();
        Lss::Top(0.2f * Lss::VH);

        static int width = 1920;
        static int height = 1080;

        Lss::SetFontSize(2.5f * Lss::VH);
        float heightSize = ImGui::CalcTextSize("Height (px): ").x;
        float WidthSize = ImGui::CalcTextSize("Width (px): ").x;

        Lss::Left(Lss::VH);
        Lss::Text("Width (px): ", 2.5f * Lss::VH);
        ImGui::SameLine();
        Lss::Left(heightSize - WidthSize + 6.5f * Lss::VW);
        Lss::InputInt("##canvasWidth", &width, ImVec2(5 * Lss::VW, 3 * Lss::VH), Rounded);

        Lss::Left(Lss::VH);
        Lss::Text("Height (px): ", 2.5f * Lss::VH);
        ImGui::SameLine();
        Lss::Left(6.5f * Lss::VW);
        Lss::InputInt("##canvasHeight", &height, ImVec2(5 * Lss::VW, 3 * Lss::VH), Rounded);

        ImVec2 buttonSize = ImVec2(100, 20);
        ImGui::SetCursorPosY(38 * Lss::VH - buttonSize.y - 2 * Lss::VH);
        if (Lss::Button("Create##createLobby", ImVec2(10 * Lss::VH, 4 * Lss::VH), 3 * Lss::VH, Centered | Rounded))
        {
            std::map<std::string, std::string> header;
            std::map<std::string, std::string> room;
            header["token"] = runtime.token;
            if (passwordText[0] != '\0') header["password"] = std::string(passwordText);
            room["name"] = nameText;
            room["create"] = "true";
            room["width"] = std::to_string(width);
            room["height"] = std::to_string(height);
            SManager::SetCanvasSize(width, height);
            std::vector<RoomUser>* roomyUsers = SManager::GetUsers();
            roomyUsers->emplace_back(runtime.id, runtime.username, true);
            SManager::SetMyOwnerState(true);
            SManager::Connect(runtime.ip.c_str(), header, room);
            DrawUI::canInit = true;
            Callback::EditorSwapCallBack(true);
        }

        if (ImGui::IsMouseClicked(0))
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 cursorPos = ImGui::GetMousePos();
            ImVec2 size = ImGui::GetWindowSize();
            if (!Lss::InBound(cursorPos, pos, size)) {
                ImGui::CloseCurrentPopup();
                openCreate = false;
            }
        }

        Lss::End();
        ImGui::EndPopup();
    }

    if (Lss::Modal("Joinyup", &openJoin, ImVec2(20 * Lss::VW, 40 * Lss::VH), Centered | Trans, ImGuiWindowFlags_NoDecoration))
    {
        static char passWordText[256];
        if (!privateLobby) {
            Lss::Text("Are you sure u want to join?", 4*Lss::VH, Centered);
            if (Lss::Button("Join", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH))
            {
                std::map<std::string, std::string> header;
                std::map<std::string, std::string> room;
                header["token"] = runtime.token;
                room["name"] = roomName;
                room["create"] = "false";
                SManager::Connect(runtime.ip.c_str(), header, room);
                Callback::EditorSwapCallBack(true);
            }
            Lss::Button("Cancel", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH, SameLine);
        } else {
            ImVec2 valid = ImGui::GetContentRegionAvail();
            Lss::Text("Please enter the room's password:", Centered);
            Lss::InputText("roomPassword", passWordText, sizeof(passWordText), ImVec2(valid.x * 0.7f, 5 * Lss::VH), Centered);
            if (Lss::Button("Join", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH)) {
                std::map<std::string, std::string> header;
                std::map<std::string, std::string> room;
                header["token"] = runtime.token;
                header["password"] = std::string(passWordText);
                room["name"] = roomName;
                room["create"] = "false";
                SManager::Connect(runtime.ip.c_str(), header, room);
                Callback::EditorSwapCallBack(true);
            }
            if (Lss::Button("Cancel", ImVec2(10 * Lss::VH, 4 * Lss::VH), 4 * Lss::VH, SameLine))
            {
                ImGui::CloseCurrentPopup();
                openJoin = false;
            }
        }

        if (ImGui::IsMouseClicked(0))
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 cursorPos = ImGui::GetMousePos();
            ImVec2 size = ImGui::GetWindowSize();
            if (!Lss::InBound(cursorPos, pos, size)) {
                ImGui::CloseCurrentPopup();
                openJoin = false;
            }
        }

        Lss::End();
        ImGui::EndPopup();
    }

    Lss::SetColor(ContainerBackground, Background);
    for (auto& room : rooms) {
        Lss::Child(room.roomName + room.ownerName, ImVec2(valid.x, 12.5f * Lss::VH), false, Centered);
        Lss::LeftTop(Lss::VW, 0.5f * Lss::VH);
        Lss::Text(room.roomName, 4 * Lss::VH);
        Lss::Top(Lss::VH);
        ImGui::SameLine();
        Lss::Top(0.5f * Lss::VH);
        Lss::Text(" - " + room.ownerName, 3 * Lss::VH);
        ImGui::SameLine();
        float buttonPos = valid.x - 6 * Lss::VW;
        ImGui::SetCursorPosX(buttonPos);
        Lss::Top(0.5f * Lss::VH);
        if (Lss::Button("Join", ImVec2(5 * Lss::VW, 5 * Lss::VH), 4 * Lss::VH, Rounded))
        {
            openJoin = true;
            privateLobby = room.password;
            roomName = room.roomName;
        }
        Lss::LeftTop(Lss::VW, Lss::VH);
        Lss::Text(std::to_string(room.userCount) + "/" + std::to_string(room.capacity), 4 * Lss::VH);
        ImGui::SameLine();
        float lockPos = valid.x - 6 * Lss::VW;
        ImGui::SetCursorPosX(lockPos);
        Lss::Text((room.password) ? "Locked" : "", 4 * Lss::VH);
        Lss::End();
        ImGui::EndChild();
        Lss::Top(Lss::VH);
    }
    Lss::SetColor(ContainerBackground, ContainerBackground);
    ImGui::PopStyleVar();
    Lss::End();
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
        MainPage(width, height);
        } break;
    case 1: { //settings
        SettingsPage();
        } break;
    case 2: { //search
        SearchPage(width, height);
        } break;
    case 3: { //view profile, add images
        ProfilePage(width, height, runtime.id);
        } break;
    case 4: {
        RoomPage(width, height);
        } break;
    default:
        break;
    }

    if (!needRooms && mode != 4) needRooms = true;
    
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
    if (runtime.logedIn) {
        Lss::Top(1 * Lss::VH);
        if (Lss::Button("Rooms", ImVec2(16 * Lss::VH, 6 * Lss::VH), 5 * Lss::VH, Invisible | Centered | Rounded)) {
            if (mode != 4) mode = 4;
            else mode = 0;
        }
    }
    Lss::Top(2 * Lss::VH);
    ImGui::Separator();

    Lss::Top(2 * Lss::VH);

    if (runtime.logedIn) {
        if (Lss::Button("Post", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
            openStuff = true;
        }
        Lss::Top(1 * Lss::VH);
    }

    Lss::Top(1 * Lss::VH);
    if (Lss::Button("Explore", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
        if (mode != 0) mode = 0;
    }

    Lss::Top(1 * Lss::VH);
    if (Lss::Button("Search", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
        if (mode != 2) mode = 2;
        else {
            if (searchMode != 0) searchMode = 0;
			else mode = 0;
            needImages = true;
        }
    }

    if (runtime.logedIn) {
        Lss::Top(1 * Lss::VH);
        if (Lss::Button("Profile", ImVec2(15 * Lss::VH, 5 * Lss::VH), 4 * Lss::VH, Invisible | Centered | Rounded)) {
            if (mode != 3) {
                mode = 3;
                userImages.clear();
                needImages = true;
            }
            else mode = 0;
        }
    }
    else {
        Lss::Top(1 * Lss::VH);
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
        /*
	    ImVec2 valid = ImGui::GetContentRegionAvail();


        Lss::Text("PopularLobbies", 5 * Lss::VH, Centered);
		Lss::Child("Lobbies", ImVec2(0, valid.y / 3 - 6 * Lss::VH), false, Centered, ImGuiWindowFlags_NoScrollbar);
        ImGui::EndChild();
	    Lss::Separator(2.0f);
		Lss::Text("Friends online", 5 * Lss::VH, Centered);
        Lss::Child("FriendsOnline", ImVec2(0, valid.y / 3 - 6 * Lss::VH), false, Centered, ImGuiWindowFlags_NoScrollbar);
        ImGui::EndChild();
        Lss::Separator(2.0f);
        Lss::Text("Friend lobbies", 5 * Lss::VH, Centered);
        Lss::Child("FriendLobbies", ImVec2(0, valid.y / 3 - 6 * Lss::VH), false, Centered, ImGuiWindowFlags_NoScrollbar);
        ImGui::EndChild();

        Lss::End();
        */
    ImGui::End();
}

std::chrono::system_clock::time_point SocialMedia::ParsePostTime(const std::string& timeData) 
{
    std::tm tm = {};
    std::istringstream ss(timeData);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    std::time_t timeUtc = std::mktime(&tm);

    std::time_t locTime = timeUtc + 3600;
    return std::chrono::system_clock::from_time_t(locTime);
}

void SocialMedia::GetPosts() 
{
    if (runtime.ip[0] == '\0') {
        if (needError) {
            std::cout << "runtimeIp not set\n";
            needError = false;
        }
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
        post.likes = postJson["likes"];
    
		for (const auto& tags : postJson["tags"]) {
			post.tags.push_back(tags);
		}

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
    try {
        std::unique_lock<std::mutex> lock(postMutex);
        std::vector<Comment> commentsHere;
        for (Comment& comment : post.comments) {
            commentsHere.push_back(comment);
        }
        lock.unlock();
        for (Comment comment : commentsHere) {
            LoadImageJa(comment.userId, 2);
        }
    }
    catch (...)
    {
        std::cerr << "error while loading commnents" << std::endl;
    }
    if (post.imageId != -1) LoadImageJa(post.imageId, 1, index);
    else post.picLoaded = true;
}

void SocialMedia::LoadImageJa(int dataId, int type, int postId)
{
    std::vector<uint8_t> imageData;
    switch (type) {
    case 1: {
        if (dataId == -1)return;
        imageData = HManager::ImageRequest(("images/" + std::to_string(dataId)).c_str());
        dataId = postId;
        } break;
    case 2: {
            if (profilePics.find(dataId) != profilePics.end()) break;
            else {
                imageData = HManager::ImageRequest(("users/" + std::to_string(dataId) + "/pfp").c_str());
            }
        } break;
    case 5: {
        //auto start = std::chrono::high_resolution_clock::now();
        imageData = HManager::ImageRequest(("images/" + std::to_string(dataId)).c_str());
        dataId = postId;

        //auto end = std::chrono::high_resolution_clock::now(); // End timing
        //std::chrono::duration<double, std::milli> duration = end - start; // Compute duration in milliseconds

        //std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
        } break;
    default:
        break;
    }
    std::lock_guard<std::mutex> queueLock(textureQueueMutex);
    textureQueue.push(std::make_tuple(std::move(imageData), dataId, type));
}