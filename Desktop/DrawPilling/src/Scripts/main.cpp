#include "Headers.h"

//Variables
int screenwidth, screenheight;

unsigned int VBO, VAO;
unsigned int CanvasVBO, CanvasVAO;

static float size = 0.1f;
bool hover = false;

unsigned int canvasX = 500, canvasY = 500;  

// Main code
int main()
{
    SessionData data;
    Manager::Assembly(data);

    NewRenderer renderer;

    Callback::Init(data.window, renderer);
    DrawUI::SetRenderer(renderer);
    SManager::SetRenderer(renderer);


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