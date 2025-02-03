#include "Headers.h"

//Variables
int screenwidth, screenheight;

unsigned int VBO, VAO;
unsigned int CanvasVBO, CanvasVAO;

static float size = 0.1f;
bool hover = false;

unsigned int canvasX = 1080, canvasY = 1080;

std::string token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im5vdGJvYiIsInN1YiI6MSwiaWF0IjoxNzM4MDc4MTczfQ.KB15yq4a6n7D5gouRj3EW5HqE0ncO67v_gfIRoUQ9Cg";


// Main code
int main()
{
    SessionData data;
    Manager::Assembly(data);

    NewRenderer renderer;

    Callback::Init(data.window, renderer);

    renderer.Init(data.window, canvasX, canvasY, data.screenWidth, data.screenHeight);

    glfwSetWindowCloseCallback(data.window, onCloseCallback);


    while (true)
    {
        glfwPollEvents();

        if (glfwGetWindowAttrib(data.window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
        }

        renderer.Render();
    }
}