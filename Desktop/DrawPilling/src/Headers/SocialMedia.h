#pragma once

#include <vector>
#include <iostream>
#include <optional>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
#include <mutex>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "HighsManager.h"
#include "Messages.h"
#include "NewRenderer.h"


struct Comment {
	int id;
	int userId;
	std::string text;
	std::string username;
	std::chrono::system_clock::time_point time;
};

struct Post
{
	int id;
	int userId;
	int imageId;
	GLuint image;
	GLuint userImage;
	float ratio;
	std::string username;
	std::string text;
	std::chrono::system_clock::time_point time;
	std::vector<Comment> comments;
};

class SocialMedia {
private:
	static std::vector<Post> posts;
	static int lastId;
public:
	static void Init(NewRenderer& rendererIn);
	static void MainFeed(float position, float width, float height);
	static void LeftSide(float position, float width, float height);
	static void RightSide(float position, float width, float height);
	static void GetPosts();
	static void LoadImages();
	static void LoadImageJa(Post* post, std::mutex& mtx, int type);
	static std::chrono::system_clock::time_point ParsePostTime(const std::string& timeData);
};