#define GLEW_STATIC
#include "GLEW/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> 

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#define CURL_STATICLIB
#include <curl/curl.h>


////////////////
//actual stuff//
////////////////

//includes
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>

#include "SessionManager.h"
#include "VertexArray.h"
#include "NewDraw.h"
#include "NewRenderer.h"
#include "CallBacks.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "json/json.hpp"

//definitions
#define PI 3.1415927f


//Variables
int screenwidth, screenheight;

unsigned int VBO, VAO;
unsigned int CanvasVBO, CanvasVAO;

static float size = 0.1f;
bool hover = false;

unsigned int canvasX = 1080, canvasY = 1080;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Main code
int main()
{
    std::string result;
    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/item");

        // Set the callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Pass the string to store response data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            try {
                // Parse the JSON data
                nlohmann::json jsonResponse = nlohmann::json::parse(result);
                // Check if there is at least one element in the JSON array
                if (!jsonResponse.empty()) {
                    // Check if the 'purity' key exists and is a valid float
                    if (jsonResponse[0].contains("purity") && jsonResponse[0]["purity"].is_string()) {
                        std::string purity = jsonResponse[0]["purity"].get<std::string>();
                        float purityNumber = std::stof(purity);
                        std::cout << "Purity: " << purityNumber << std::endl;
                    }
                    else {
                        std::cerr << "'purity' key missing or not a number" << std::endl;
                    }
                }
                else {
                    std::cerr << "JSON array is empty" << std::endl;
                }
            }
            catch (const nlohmann::json::exception& e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }

        // Clean up the curl session
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    SessionData data = Manager::Assembly();

    NewRenderer renderer;

    Callback::Init(data.window, renderer);

    renderer.Init(data.window, canvasX, canvasY, data.screenWidth, data.screenHeight);

    // Main loop
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(data.window))
#endif
    {
        //manage callbacks
        glfwPollEvents();

        if (glfwGetWindowAttrib(data.window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
        }

        //render stuff
        renderer.Render();

    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    //Manager::DisAssembly(data.window, shaderAndLocs.shader);
}