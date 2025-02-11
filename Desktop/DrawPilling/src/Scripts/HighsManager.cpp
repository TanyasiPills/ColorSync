#include "HighsManager.h"

CURL* curl;

CURLcode res;
std::string result;
struct curl_slist* headers;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void HManager::Init()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
	}
	else {
		std::cout << "HManager cant be initialized" << std::endl;
	}
}

void HManager::Down() 
{
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

nlohmann::json HManager::Request(std::string query, std::string body, Method method)
{
	if (curl) {
		std::string url = "http://" + query;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		if (method == GET) {
			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, nullptr);
		}
		else if (method == DEL) {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		}
		else {
			if (method == PATCH) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
			}
			if (method == POST)
			{
				curl_easy_setopt(curl, CURLOPT_POST, 1L);
			}
			if (body.empty()) std::cerr << "Post/Patch sent with no body" << std::endl;

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		res = curl_easy_perform(curl);

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

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
