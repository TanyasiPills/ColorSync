#pragma once
#include <string>
#include <iostream>
#define CURL_STATICLIB
#include "curl/curl.h"
#include "json/json.hpp"

class HManager
{
private:

public:
	static void Init();
	static void Down();
	nlohmann::json Request(std::string query, std::string body);
};