#define GLEW_STATIC
#include "GLEW/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <sstream>
#include <iostream>

#include "old/NCallBacks.h"
#include "old/Renderer.h"
#include "old/Shader.h"

unsigned int* VBOH;
unsigned int* VAOH;
unsigned int* canVAO;
unsigned int* canVBO;
float* sizeH;
bool* hoverH;

static bool spacePressed = false;

static int initialWidth = -1;
static int initialHeight = -1;
static int previousWidth = 800;
static int prevousHeight = 800;
static int previousX = 0;
static int previousY = 0;

static float xRatio = 1.0f;
static float yRatio = 1.0f;
static float xOffset = 0.0f;
static float yOffset = 0.0f;
static float scale = 1.0f;

GLint xAspectLoc;
GLint yAspectLoc;
GLint xOffsetLoc;
GLint yOffsetLoc;
GLint scaleLoc;

static double previousMousePosX = -1;
static double previousMousePosY = -1;
static double initx = -1;
static double inity = -1;
static float prevScale = 1.0f;
/*
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    double deltaX = 0.5 - (xpos / width);
    double deltaY = 0.5 - (ypos / height);
    if (std::abs(deltaX) > 0.3) deltaX = (deltaX > 0) ? 0.3 : -0.3;
    if (std::abs(deltaY) > 0.3) deltaY = (deltaY > 0) ? 0.3 : -0.3;

    if (yoffset > 0) {
        prevScale = scale;
        scale *= 1.12f;
        if (scale > 2.0f) {
            scale = prevScale;
            return;
        }

        glUniform1f(scaleLoc, scale);

        xOffset += deltaX / 2;
        yOffset -= deltaY / 2;
        glUniform1f(xOffsetLoc, xOffset);
        glUniform1f(yOffsetLoc, yOffset);
    }
    else if (yoffset < 0) {
        prevScale = scale;
        scale *= 0.89285f;
        if (scale < 0.6f) {
            scale = prevScale;
            return;
        }

        glUniform1f(scaleLoc, scale);

        xOffset -= deltaX / 2;
        yOffset += deltaY / 2;
        glUniform1f(xOffsetLoc, xOffset);
        glUniform1f(yOffsetLoc, yOffset);
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    if (initialWidth == -1) {
        initialWidth = width;
        initialHeight = height;
    }

    if (width != previousWidth) {
        xRatio = initialWidth / static_cast<float>(width);
        previousWidth = width;
        glUniform1f(xAspectLoc, xRatio);
    }
    if (height != prevousHeight) {
        yRatio = initialHeight / static_cast<float>(height);
        prevousHeight = height;
        glUniform1f(yAspectLoc, yRatio);
    }

    glViewport(0, 0, width, height);
    Renderer::RenderScreen(window, *VBOH, *VAOH, sizeH, hoverH, *canVBO, *canVAO);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
*/
Ratios CallBackManager::SetCallBacks(GLFWwindow* window, unsigned int* VBO, unsigned int* VAO, float* size, bool* hover, ShaderAndLocs& shaderAndLocs, unsigned int* canvasVBO, unsigned int* canvasVAO)
{ 
    VBOH = VBO;
    VAOH = VAO;
    canVAO = canvasVAO;
    canVBO = canvasVBO;
    sizeH = size;
    hoverH = hover;

    xAspectLoc = shaderAndLocs.xRatio;
    yAspectLoc = shaderAndLocs.yRatio;
    xOffsetLoc = shaderAndLocs.xOffset;
    yOffsetLoc = shaderAndLocs.yOffset;
    scaleLoc = shaderAndLocs.scale;

    Ratios mapData;
    mapData.xRatio = &xRatio;
    mapData.yRatio = &yRatio;
    mapData.xOffset = &xOffset;
    mapData.yOffset = &yOffset;
    mapData.scale = &scale;

    glUniform1f(xAspectLoc, xRatio);
    glUniform1f(yAspectLoc, yRatio);
    glUniform1f(xOffsetLoc, xOffset);
    glUniform1f(yOffsetLoc, yOffset);
    glUniform1f(scaleLoc, scale);
    /*
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetErrorCallback(glfw_error_callback);
    */

    return mapData;
}

void CallBackManager::ProcessInput(GLFWwindow* window) 
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressed) {
            glfwGetCursorPos(window, &initx, &inity);
            glfwGetCursorPos(window, &previousMousePosX, &previousMousePosY);
            spacePressed = true;
            return;
        }

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (xpos != previousMousePosX) {
            float xdiff = (xpos - previousMousePosX) / width;
            //std::cout << (xpos - initx) / width << std::endl;
            xOffset += xdiff*2;
            glUniform1f(xOffsetLoc, xOffset);
        }
        if (ypos != previousMousePosY) {
            float ydiff = (ypos - previousMousePosY) / height;
            //std::cout << (ypos - inity) / height << std::endl;
            yOffset -= ydiff*2;
            glUniform1f(yOffsetLoc, yOffset);
        }

        previousMousePosX = xpos;
        previousMousePosY = ypos;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        if (spacePressed) {
            spacePressed = false;
        }
    }
}