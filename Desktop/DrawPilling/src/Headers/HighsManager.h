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

class HManager
{
private:

public:
	static void Init();
	static void Down();
	static nlohmann::json Request(std::string query, std::string body, Method method, std::string tokenIn = "");
	static std::vector<uint8_t> Request(std::string query, Method method);
	static GLuint ImageFromRequest(const std::vector<uint8_t>& imageData, float& ratio);
	static std::vector<uint8_t> ImageFormFiles(const std::string& filePath);
};