#include "HighsManager.h"

CURLcode res;
struct curl_slist* headers;

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
void HManager::Init()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

void HManager::Down() 
{
	curl_global_cleanup();
}

nlohmann::json HManager::Request(std::string query, std::string body, Method method)
{
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	if (curl) {
		std::string url = "http://" + query;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);


		std::string result;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


		if (method == GET) {
			curl_easy_setopt(curl, CURLOPT_HTTPGET, 10L);
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


		std::cout << "Requesting URL: " << url << std::endl;
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
		curl_easy_cleanup(curl);
	}
	else {
		std::cerr << "HManager isn't initialized" << std::endl;
		return nlohmann::json{};
	}
}

std::vector<uint8_t> HManager::Request(const std::string query, Method method)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Curl not initialized!" << std::endl;
		return {};
	}

	std::vector<uint8_t> imageData;
	std::string url = "http://" + query;
	if (url.find("-1") != std::string::npos) return imageData;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	if (method == GET) {
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	}
	else {
		std::cerr << "Only GET method is supported for images." << std::endl;
		return {};
	}

	std::cout << "Requesting URL: " << url << std::endl;

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ImageWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		curl_easy_cleanup(curl);  // Clean up the CURL handle for each thread
		return {};
	}

	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	std::cout << "HTTP Response Code: " << http_code << std::endl;

	curl_easy_cleanup(curl);  // Clean up after request
	return imageData;
}

GLuint HManager::ImageFromRequest(const std::vector<uint8_t>& imageData, float& ratio) {

	if (imageData.empty()) {
		std::cerr << "Error: imageData is empty" << std::endl;
		return -1;
	}

	int width, height, channels;
	unsigned char* data = stbi_load_from_memory(imageData.data(), imageData.size(), &width, &height, &channels, 3);

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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

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

std::vector<uint8_t> HManager::ImageFormFiles(const std::string& filePath)
{
	int width, height, channels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 3);
	size_t dataSize = width * height * 3;
	std::vector<uint8_t> imageData(data, data + dataSize);

	stbi_image_free(data);
	return imageData;
}