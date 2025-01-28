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

//definitions
#define PI 3.1415927f


//Variables
int screenwidth, screenheight;

unsigned int VBO, VAO;
unsigned int CanvasVBO, CanvasVAO;

static float size = 0.1f;
bool hover = false;

unsigned int canvasX = 1080, canvasY = 1080;



// Main code
int main()
{
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