#pragma once
#include <memory>

struct SessionData
{
	int screenWidth;
	int screenHeight;
	GLFWwindow* window;
};

class Manager {
public:
	static SessionData Assembly(SessionData& data);
	static void DisAssembly(GLFWwindow* window);
};
