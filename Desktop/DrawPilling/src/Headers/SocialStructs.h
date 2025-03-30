#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <ctime>
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
	int size = 0;
	int likes = 0;
	GLuint image = -1;
	float ratio;
	std::string text;
	std::chrono::system_clock::time_point time;
	std::vector<Comment> comments;
	std::vector<std::string> tags;
	bool picLoaded = false;
	bool allLoaded = false;
	bool openComments = false;
	bool needChange = true;
	bool liked = false;
	bool likeHovered = false;
	bool commentHovered = false;
};

struct User
{
	std::string username;
	GLuint userImage = -1;
	std::string bio;
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

struct Room
{
	std::string roomName;
	std::string ownerName;
	int capacity;
	int ownerId;
	int userCount;
	bool password;
};