#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <shlwapi.h>
#include <algorithm>
#include "GLEW/glew.h"


class Explorer
{
private:
public:
	static void Init();
	static void FileExplorerUI(bool* creatorStuff, int idForFormat = 3);
	static std::vector<std::string> GetFilesInDirectory(const std::string& directory);
	static std::string GetImagePath();
};