#pragma once

struct SessionData
{
	int screenWidth;
	int screenHeight;
	GLFWwindow* window;
};

class Manager {
public:
	static SessionData Assembly();
	static void DisAssembly(GLFWwindow* window, unsigned int shader);
};
