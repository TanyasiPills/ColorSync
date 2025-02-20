#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <iostream>
#include "GLEW/glew.h"


class Explorer
{
private:
public:
	static void Init(GLuint folderIn, GLuint folderFullIn, GLuint fileIdIn);
	static void FileExplorerUI();
	static std::vector<std::string> GetFilesInDirectory(const std::string& directory);
};