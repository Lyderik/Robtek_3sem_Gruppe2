#pragma once
#include <vector>
#include <dirent.h>
#include <string>
#include <Windows.h>
#include "TreeElement.h"
#include <fstream>

using namespace System::Windows::Forms;
using namespace System;

class DirectoryHandler
{
public:
	DirectoryHandler();
	static std::vector<std::string> getDirectories(const char * directoryName);
	static std::vector<std::string> getFiles(const char * directoryName);
	static std::vector<TreeElement> getAllFolders(const char  * Path);
	static std::vector<TreeElement> convertToTreeElements(std::string Path, std::vector<std::string> folderList);
	static std::vector<std::string> getAllFiles(std::vector<TreeElement> folderList);
	static std::vector<std::string> loadFromFile(std::string Path);
	~DirectoryHandler();
};

