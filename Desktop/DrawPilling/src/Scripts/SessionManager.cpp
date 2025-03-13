#include "GLEW/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> 

#include "SessionManager.h"
#include "HighsManager.h"
#include "DataManager.h"
#include "lss.h"
#include "SocialMedia.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <windows.h>

#include "DrawUI.h"
#include "FileExplorer.h"
#include "WindowManager.h"


SessionData Manager::Assembly(SessionData& data) {


    glfwInit(); //!!!!!!!!

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // No window borders
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // No resizing

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "I hate Jazz", nullptr, nullptr);
    //WindowManager::ToggleFullscreen(window);


    data.window = window;
    data.screenWidth = screenWidth;
    data.screenHeight = screenHeight;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glewInit(); //!!!!!!!!

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->Clear();
    //ImGui::PushFont(hihi);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_BLEND);
   // glBlendFunc(GL_ONE, GL_ZERO);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    DataManager::LoadAppData();

    HManager::Init();
    Lss::Init(window, screenWidth, screenHeight);

    Explorer::Init();

    WindowManager::ToggleFullscreen(window);


    return data;
}
void Manager::DisAssembly(GLFWwindow* window) {
    DataManager::SaveAppData();
    HManager::Down();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}