#include "lss.h"

static GLFWwindow* window = nullptr;
float vW = 0;
float vH = 0;

void Lss::Init(GLFWwindow* windowIn) {
	window = windowIn;
}

void Lss::Update() {
	int width, height;
	glfwGetWindowSize(window,&width, &height);
	vW = width / 100.0;
	vH = height / 100.0;
}

int Lss::VW() {
	if (vW < 1) return 1;
	else return (int)vW;
}

int Lss::VH() {
	if (vH < 1) return 1;
	else return (int)vH;
}



