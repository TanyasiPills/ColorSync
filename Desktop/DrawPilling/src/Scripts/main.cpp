#include "Headers.h"


// Main code
//set up the scripts then loop rendering
int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    SessionData data;
    Manager::Assembly(data);

    NewRenderer renderer;

    Callback::Init(data.window, renderer);
    DrawUI::SetRenderer(renderer);
    SManager::SetRenderer(renderer);
    DataManager::SetRenderer(renderer);

    SocialMedia::LoadTextures();

    renderer.Init(data.window);


    while (true)
    {
        glfwPollEvents();

        if (glfwGetWindowAttrib(data.window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
        }
        else {
            renderer.Render();
        }
    }
}