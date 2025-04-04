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
	static bool Exists();
	static void FileExplorerUI(bool* creatorStuff, int idForFormat = 2);
	static std::vector<std::string> GetFilesInDirectory(const std::string& directory);
	static std::string GetImagePath();
};