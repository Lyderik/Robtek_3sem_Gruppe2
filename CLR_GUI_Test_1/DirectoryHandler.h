#pragma once
#include <vector>
#include "dirent.h""
#include <string>
#include <Windows.h>
#include "TreeElement.h"

using namespace System::Windows::Forms;
using namespace System;

class DirectoryHandler
{
public:
	DirectoryHandler();
	std::vector<std::string> getDirectories(const char * directoryName);
	std::vector<std::string> getFiles(const char * directoryName);
	std::vector<TreeElement> getAllFolders(const char  * Path);
	std::vector<TreeElement> convertToTreeElements(std::string Path, std::vector<std::string> folderList);
	std::vector<std::string> getAllFiles(std::vector<TreeElement> folderList);
	~DirectoryHandler();
};

