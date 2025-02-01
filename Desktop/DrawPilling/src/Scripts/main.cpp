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
#include "HighsManager.h"

#include "SocketSource/sio_client.h"

//definitions
#define PI 3.1415927f


//Variables
int screenwidth, screenheight;

unsigned int VBO, VAO;
unsigned int CanvasVBO, CanvasVAO;

static float size = 0.1f;
bool hover = false;

unsigned int canvasX = 1080, canvasY = 1080;

std::string token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im5vdGJvYiIsInN1YiI6MSwiaWF0IjoxNzM4MDc4MTczfQ.KB15yq4a6n7D5gouRj3EW5HqE0ncO67v_gfIRoUQ9Cg";


void onCloseCallback(GLFWwindow* window) {
    std::cout << "Window close event intercepted. Doing necessary cleanup before closing.\n";
    Manager::DisAssembly(window);
}

// Main code
int main()
{
    SessionData data;
    Manager::Assembly(data);

    NewRenderer renderer;

    Callback::Init(data.window, renderer);

    renderer.Init(data.window, canvasX, canvasY, data.screenWidth, data.screenHeight);

    glfwSetWindowCloseCallback(data.window, onCloseCallback);


    while (!glfwWindowShouldClose(data.window))
    {
        glfwPollEvents();

        if (glfwGetWindowAttrib(data.window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
        }

        renderer.Render();
    }
}