#pragma once
#include <string>
#include <chrono>
#include <vector>
#include "GLEW/glew.h"

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