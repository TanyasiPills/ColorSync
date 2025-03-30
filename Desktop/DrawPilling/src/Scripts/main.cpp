#include "Headers.h"

// Main code
int main()
{
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

        renderer.ProcessTasks();
        renderer.Render();
    }
}