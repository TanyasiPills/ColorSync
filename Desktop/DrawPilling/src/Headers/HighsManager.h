#pragma once
#include <string>
#include <iostream>
#define CURL_STATICLIB
#include "curl/curl.h"
#include "json/json.hpp"

class HManager
{
private:
	CURL* curl;
public:
	HManager();
	~HManager();
	nlohmann::json Request(std::string query);
};