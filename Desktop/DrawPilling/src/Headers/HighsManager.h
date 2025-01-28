#pragma once
#include <string>
#include <iostream>
#define CURL_STATICLIB
#include "curl/curl.h"
#include "json/json.hpp"

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
	static nlohmann::json Request(std::string query, std::string body, Method method);
};