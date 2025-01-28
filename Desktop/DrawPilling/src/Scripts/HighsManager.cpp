#include "HighsManager.h"

CURLcode res;
std::string result;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

HManager::HManager()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
	}
	else {
		std::cout << "HManager cant be initialized" << std::endl;
	}
}

HManager::~HManager() 
{
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

nlohmann::json HManager::Request(std::string query)
{
	if (curl) {
		std::string url = "http://" + query;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
			return nlohmann::json{};
		} 
		else {
			try {
				nlohmann::json jsonResponse = nlohmann::json::parse(result);
				if (!jsonResponse.empty()) {
					return jsonResponse;
				}
				else {
					std::cerr << "JSON array is empty" << std::endl;
				}
			}
			catch (const nlohmann::json::exception& e) {
				std::cerr << "Error parsing JSON: " << e.what() << std::endl;
			}
		}
	}
	else {
		std::cerr << "HManager isn't initialized" << std::endl;
		return nlohmann::json{};
	}
}
