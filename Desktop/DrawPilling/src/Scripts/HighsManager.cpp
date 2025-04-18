#include "HighsManager.h"
#include "SocialMedia.h"

CURLcode res;
struct curl_slist* headers;

static auto& runtime = RuntimeData::getInstance();

class SocialMedia;


size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

size_t ImageWriteCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
	auto& buffer = *reinterpret_cast<std::vector<uint8_t>*>(userdata);
	size_t totalSize = size * nmemb;
	buffer.insert(buffer.end(), static_cast<uint8_t*>(ptr), static_cast<uint8_t*>(ptr) + totalSize);
	return totalSize;
}


void HManager::InitUser()
{
	if (runtime.token[0] != '\0') {
		try {
			std::cout << "Your token, good sir: " << runtime.token << std::endl;
			nlohmann::json result = Request("users", "", GET);
			if (result.empty()) {
				std::cerr << "Error: Received empty response from server." << std::endl;
				runtime.logedIn = false;
				return;
			}
			if (result.contains("statusCode") && result["statusCode"] == 401) {
				runtime.logedIn = false;
			}
			else if (result.contains("id") && result.contains("username") && result.contains("email")) {

				runtime.id = result["id"];
				runtime.username = result["username"];
				runtime.email = result["email"];

				std::vector<uint8_t> imageData = ImageRequest(("users/" + std::to_string(runtime.id) + "/pfp").c_str());

				if (imageData.empty()) {
					std::cerr << "Failed to fetch profile picture." << std::endl;
				}
				else {
					float ratioStuff = 0.0f;
					auto* textureQueue = SocialMedia::GetTextureQueue();
					textureQueue->push(std::make_tuple(imageData, 0, 3));
				}
				result = HManager::Request("users/likes", "", GET);
				if(!result.is_null() && !result.empty())for (auto& item : result) runtime.liked.insert((int)item);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Request failed: " << e.what() << std::endl;
			runtime.logedIn = false;
		}
	}
	else {
		runtime.logedIn = false;
	}
}

void HManager::Init()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	std::thread(&HManager::InitUser).detach();
}

void HManager::Down() 
{
	curl_global_cleanup();
}


nlohmann::json HManager::PostRequest(std::string text, std::string path, int imageId, std::vector<std::string> tags, bool forcePost)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "HManager isn't initialized" << std::endl;
		return nlohmann::json{};
	}


	std::string url = "http://" + runtime.ip+":3000/posts";
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	struct curl_slist* headers = nullptr;
	if (!runtime.token[0] == '\0') {
		std::string authHeader = "Authorization: Bearer " + runtime.token;
		headers = curl_slist_append(headers, authHeader.c_str());
	}
	else std::cerr << "No valid token found" << std::endl;
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string result;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);


	curl_mime* mime = curl_mime_init(curl);
	curl_mimepart* part = nullptr;

	part = curl_mime_addpart(mime);
	curl_mime_name(part, "text");
	curl_mime_data(part, text.c_str(), CURL_ZERO_TERMINATED);

	if (path != "") {
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "file");
		curl_mime_filedata(part, path.c_str());
	}
	else if (imageId > 0) {
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "imageId");
		curl_mime_data(part, std::to_string(imageId).c_str(), CURL_ZERO_TERMINATED);
	}

	if (forcePost) {
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "forcePost");
		curl_mime_data(part, "1", CURL_ZERO_TERMINATED);
	}

	if (!tags.empty()) {
		nlohmann::json tagsJson = nlohmann::json::array();
		for (const auto& tag : tags) {
			tagsJson.push_back(tag);
		}

		std::string tagsString = tagsJson.dump();
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "tags");
		curl_mime_type(part, "application/json");
		curl_mime_data(part, tagsString.c_str(), tagsString.size());
	}

	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);


	CURLcode res;
	long http_code = 0;

	res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);


	curl_mime_free(mime);
	curl_easy_cleanup(curl);


	if (http_code != 200 && http_code != 201)
	{
		std::cerr << "Request failed with HTTP code: " << http_code << std::endl;
		try {
			nlohmann::json jsonResponse = nlohmann::json::parse(result);
			std::cerr << "Error with request: " << jsonResponse.dump() << std::endl;
		}
		catch (...) {
			std::cerr << "no body for error" << std::endl;
		}

		//return nlohmann::json::parse(result);
		return nullptr;
	}
	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		return nullptr;
	}
	else {
		try {
			nlohmann::json jsonResponse = nlohmann::json::parse(result);
			if (!jsonResponse.empty()) {
				return jsonResponse;
			}
			else {
				std::cerr << "JSON response is empty" << std::endl;
			}
		}
		catch (const nlohmann::json::exception& e) {
			std::cerr << "Error parsing JSON: " << e.what() << std::endl;
		}
	}

	return nlohmann::json{};
}

nlohmann::json HManager::ImageUploadRequest(std::string path, int type, std::string visibility)
{
	std::string fileName = path.substr(path.find_last_of('\\') + 1);

	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "HManager isn't initialized" << std::endl;
		return nlohmann::json{};
	}
	std::string url;
	if (type == 0) url = "http://" + runtime.ip + ":3000/images";
	else url = "http://" + runtime.ip + ":3000/users/pfp";
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	struct curl_slist* headers = nullptr;
	std::string authHeader = "Authorization: Bearer " + runtime.token;
	headers = curl_slist_append(headers, authHeader.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string result;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

	curl_mime* mime = curl_mime_init(curl);
	curl_mimepart* part = curl_mime_addpart(mime);
	curl_mime_name(part, "file");

	curl_mime_filename(part, fileName.c_str());

	curl_mime_filedata(part, path.c_str());

	if (fileName.find(".png") != std::string::npos) {
		curl_mime_type(part, "image/png");
	}
	else if (fileName.find(".jpg") != std::string::npos || fileName.find(".jpeg") != std::string::npos) {
		curl_mime_type(part, "image/jpeg");
	}

	if (!visibility.empty()) {
		curl_mimepart* visibilityPart = curl_mime_addpart(mime);
		curl_mime_name(visibilityPart, "visibility");
		curl_mime_data(visibilityPart, visibility.c_str(), CURL_ZERO_TERMINATED);
	}


	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200 && http_code != 201)
	{
		std::cerr << "Get voided bitch" << std::endl;
		return nullptr;
	}

	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		return nullptr;
	}
	else {
		try {
			return nlohmann::json::parse(result);
		}
		catch (const nlohmann::json::exception& e) {
			std::cerr << "Error parsing JSON: " << e.what() << std::endl;
		}
	}

	curl_easy_cleanup(curl);
	if (headers) curl_slist_free_all(headers);

	return nlohmann::json{};
}

nlohmann::json HManager::Request(std::string query, std::string body, Method method)
{
	CURL* curl = curl_easy_init();
	if (curl) {
		std::string url = "http://" +runtime.ip+":3000/" + query;
		std::cout << "URL: " << url << std::endl;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 250L);


		std::string result;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		if (runtime.token[0] != '\0') {
			std::string authHeader = "Authorization: Bearer " + runtime.token;
			headers = curl_slist_append(headers, authHeader.c_str());
		}
		else std::cerr << "No token in runtime; Request: " << query << " sent without token" << std::endl;
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


		if (method == GET) {
			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
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
			if (body.empty()) {
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}");
				std::cerr << "Post/Patch sent with no body" << std::endl;
			}
			else curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		}


		res = curl_easy_perform(curl);
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		if (res != CURLE_OK)
		{
			std::cerr << "Curl failed nya: " << curl_easy_strerror(res) << std::endl;
		}

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);


		std::cout << http_code << std::endl;

		if (http_code != 200 && http_code != 201 && http_code != 204)
		{
			/*
			nlohmann::json jsonResponse = nlohmann::json::parse(result);
			std::cout << jsonResponse.dump(4) << std::endl;
			*/
			std::cerr << "Get voided bitch" << std::endl;
			return nullptr;
		}
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
			return nullptr;
		} 
		else {
			try {
				if (result.empty()) {
					std::cerr << "Response body is empty" << std::endl;
					return nlohmann::json{};
				}
				else {
					nlohmann::json jsonResponse = nlohmann::json::parse(result);
					if (!jsonResponse.empty()) {
						//std::cout << jsonResponse.dump(4) << std::endl;
						return jsonResponse;
					}
					else {
						std::cerr << "JSON array is empty" << std::endl;
						return nullptr;
					}
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


std::vector<uint8_t> HManager::ImageRequest(const std::string query)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Curl not initialized!" << std::endl;
		return {};
	}

	std::vector<uint8_t> imageData;
	std::string url = "http://" + runtime.ip + ":3000/" + query;
	if (url.find("-1") != std::string::npos) return imageData;

	struct curl_slist* headers = nullptr;
	std::string authHeader = "Authorization: Bearer " + runtime.token;
	headers = curl_slist_append(headers, authHeader.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
	curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 60L);


	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ImageWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		curl_easy_cleanup(curl);
		return {};
	}

	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	/*
	double downloadSpeed;
	curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &downloadSpeed);
	std::cout << "Download Speed: " << downloadSpeed << " bytes per second" << std::endl;

	double startTransferTime;
	curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &startTransferTime);
	std::cout << "Time to Start Transfer: " << startTransferTime << " seconds" << std::endl;

	double totalTime;
	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &totalTime);
	std::cout << "Total Time: " << totalTime << " seconds" << std::endl;

	double transferTime = totalTime - startTransferTime;
	std::cout << "Transfer Time (Time to download the image): " << transferTime << " seconds" << std::endl;

	double dnsTime, connectTime;
	curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &dnsTime);
	curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connectTime);
	std::cout << "DNS Lookup Time: " << dnsTime << " seconds" << std::endl;
	std::cout << "Connection Time: " << connectTime << " seconds\n" << std::endl;
	*/

	curl_easy_cleanup(curl);
	return imageData;
}

GLuint HManager::ImageFromRequest(const std::vector<uint8_t>& imageData, float& ratio) {

	if (imageData.empty()) {
		std::cerr << "Error: imageData is empty" << std::endl;
		return -1;
	}

	int width, height, channels;
	unsigned char* data = stbi_load_from_memory(imageData.data(), imageData.size(), &width, &height, &channels, 4);

	if (!data) {
		std::cerr << "Failed to load image from memory" << std::endl;
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	if (texture == 0) {
		std::cerr << "Error: glGenTextures failed!" << std::endl;
		stbi_image_free(data);
		return 0;
	}
	glBindTexture(GL_TEXTURE_2D, texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << err << std::endl;
	}


	ratio = abs((float)height / width);
	if (ratio < 0)
		std::cout << height << "; " << width << std::endl;
	return texture;
}