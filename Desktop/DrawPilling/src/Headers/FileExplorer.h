#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <shlwapi.h>
#include <algorithm>
#include "GLEW/glew.h"


struct FolderSave {
	std::string name;
	std::string path;

	FolderSave() = default;

	FolderSave(std::string nameIn, std::string pathIn)
		: name(nameIn), path(pathIn) {
	}
};


class Explorer
{
private:
public:
	static void Init();
	static bool Exists();
	static void FileExplorerUI(bool* creatorStuff, int idForFormat = 2);
	static std::vector<std::string> GetFilesInDirectory(const std::string& directory);
	static std::string GetImagePath();
	static std::vector<FolderSave>* GetFavorites();
	static std::vector<FolderSave>* GetRecents();
	static void SetFavorites(std::vector<FolderSave> fvs);
	static void SetRecents(std::vector<FolderSave> rcnts);
};