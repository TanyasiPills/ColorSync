//#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <windows.h>
#include "WindowManager.h"

bool titlebar = false;
int windowedX, windowedY, windowedWidth, windowedHeight;

void WindowManager::ToggleFullscreen(GLFWwindow* window) {
    /*
    HWND hwnd = glfwGetWin32Window(window);
    glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
    if (!titlebar) {
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style &= ~WS_OVERLAPPEDWINDOW;
        SetWindowLong(hwnd, GWL_STYLE, style);
        ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    }
    else {      
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style |= WS_OVERLAPPEDWINDOW;
        SetWindowLong(hwnd, GWL_STYLE, style);    
        ShowWindow(hwnd, SW_SHOWDEFAULT);
    }

    titlebar = !titlebar;
    */
}


