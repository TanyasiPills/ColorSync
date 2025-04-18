#pragma once

#define IMGUI_STATIC
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
#include <windows.h>

#include "SessionManager.h"
#include "VertexArray.h"
#include "NewDraw.h"
#include "NewRenderer.h"
#include "CallBacks.h"
#include "HighsManager.h"

#include "SocketSource/sio_client.h"

#include "DrawUI.h"
#include "SocksManager.h"
#include "SocialMedia.h"
#include "DataManager.h"

//definitions
#define PI 3.1415927f
