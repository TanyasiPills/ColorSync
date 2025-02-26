#pragma once

#include <vector>
#include <iostream>
#include <optional>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
#include <mutex>
#include <queue>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "HighsManager.h"
#include "Messages.h"
#include "SocialStructs.h"





class SocialMedia {
private:
	static std::vector<Post> posts;
	static int lastId;
public:
	static void MainFeed(float position, float width, float height);
	static void LeftSide(float position, float width, float height);
	static void RightSide(float position, float width, float height);
	static void GetPosts();
	static void LoadDependencies(Post& post, int index);
	static void LoadImages();
	static void LoadImageJa(int dataId, int type, int postId = 0);
	static std::chrono::system_clock::time_point ParsePostTime(const std::string& timeData);
	static void ProcessThreads();
};