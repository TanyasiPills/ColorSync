#pragma once
#include <string>
#include <iostream>
#define CURL_STATICLIB
#include "curl/curl.h"
#include "json/json.hpp"
#include <vector>

#include "GLEW/glew.h"
#include "stb_image/stb_image.h"

#include "RuntimeData.h"

enum Method {
	GET,
	POST,
	DEL,
	PATCH
};

class SocialMedia;

class HManager
{
private:

public:
	static void Init();
	static void InitUser();
	static void Down();
	static nlohmann::json Request(std::string query, std::string body, Method method);
	static nlohmann::json ImageUploadRequest(std::string path, int type);
	static std::vector<uint8_t> ImageRequest(const std::string query);
	static GLuint ImageFromRequest(const std::vector<uint8_t>& imageData, float& ratio);
	static nlohmann::json PostRequest(std::string text, std::string path = "", int imageId = 0, std::vector<std::string> tags = {}, bool forcePost = false);
};