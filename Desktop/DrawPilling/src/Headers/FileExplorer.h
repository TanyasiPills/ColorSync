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
	static void Init();
	static void FileExplorerUI(bool* creatorStuff);
	static std::vector<std::string> GetFilesInDirectory(const std::string& directory);
};