#pragma once
#include <string>
#include <chrono>
#include <vector>
#include "GLEW/glew.h"

struct Comment 
{
	int id;
	int userId;
	std::string text;
	std::chrono::system_clock::time_point time;
};

struct Post
{
	int id;
	int userId;
	int imageId;
	GLuint image = -1;
	float ratio;
	std::string text;
	std::chrono::system_clock::time_point time;
	std::vector<Comment> comments;
	bool picLoaded = false;
	bool allLoaded = false;
	bool openComments = true;
	int size = 0;
};

struct User
{
	std::string username;
	GLuint userImage = -1;
	bool pPicLoaded = false;
};

struct Lobby
{
	std::string lobbyName;
	int capacity;
	int connectivity;
};

struct Image
{
	int imageId;
	float ratio;
};