#include "DirectoryHandler.h"



DirectoryHandler::DirectoryHandler()
{
}

std::vector<std::string> DirectoryHandler::getDirectories(const char * directoryName)
{
	std::vector<std::string> contentList; //Makes output list
	DIR * dir; //Creates DIR
	struct dirent * ent; //Creates a dirent struct
	int inc = 0; //Declares iterator (Used to ignore first two elements)

	if ((dir = opendir(directoryName)) != NULL) //Opens directory from path
	{
		while ((ent = readdir(dir)) != NULL) //While there are elements in the directory
		{
			if (ent->d_type == DT_DIR) //If the element is a directory
			{
				if (inc > 1) //If the element is the third or higher
				{
					contentList.push_back(ent->d_name); //Add directory to output list
				}
				inc++;				
			}
		}
		closedir(dir); //Closes directory again
	}
	return contentList;
}

std::vector<std::string> DirectoryHandler::getFiles(const char * directoryName)
{
	std::vector<std::string> contentList; //Makes output list
	DIR * dir; //Creates DIR
	struct dirent *ent; //Creates a dirent struct

	if ((dir = opendir(directoryName)) != NULL) //Opens directory from path
	{
		while ((ent = readdir(dir)) != NULL) //while there are elements in the directory
		{
			if (ent->d_type == DT_REG) //If the element is a file
			{
				contentList.push_back(ent->d_name); //Add file to output list
			}
		}
		closedir(dir); //Closes directory again
	}
	return contentList;
}

std::vector<TreeElement> DirectoryHandler::getAllFolders(const char * Path)
{
	std::vector<TreeElement> folderList; //Makes list for output
	std::vector<TreeElement> unchecked = convertToTreeElements(Path, getDirectories(Path)); //Makes and populates comparison list 

	while (unchecked.size() > 0) //As long as there are unchecked directories
	{
		std::vector<TreeElement> t = convertToTreeElements(unchecked[0].path, getDirectories(unchecked[0].path.c_str())); //Creates list of tree elements from first unchecked directory
		for (int i = 0; i < t.size(); i++) //Loops through all found directories
		{
			unchecked.push_back(t[i]); //Adds directory to unchecked list
		}
		folderList.push_back(unchecked[0]); //Adds the recently "checked" directory to output list
		unchecked.erase(unchecked.begin()); //Removes the first element of unchecked list
	}
	return folderList;
}

std::vector<TreeElement> DirectoryHandler::convertToTreeElements(std::string Path, std::vector<std::string> folderList)
{
	std::vector<TreeElement> convertedList; //Creates an output list of TreeElements
	for (int i = 0; i < folderList.size(); i++) //Loops through given list of strings
	{
		convertedList.push_back(TreeElement(Path, folderList[i],true)); //Adds TreeElement from given path and string
	}
	return convertedList;
}

std::vector<std::string> DirectoryHandler::getAllFiles(std::vector<TreeElement> folderList)
{
	std::vector<std::string> fileList;
	for (int i = 0; i < folderList.size(); i++)
	{
		std::vector<std::string> tempFiles = getFiles(folderList[i].path.c_str());
		for (int j = 0; j < tempFiles.size(); j++)
		{
			std::string s = folderList[i].path + "\\" + tempFiles[j];
			fileList.push_back(s);
		}		
	}
	return fileList;
}

std::vector<std::string> DirectoryHandler::loadFromFile(std::string Path)
{
	std::vector<std::string> stringList;
	std::ifstream stream(Path);
	while (stream.good())
	{
		char line[512];
		stream.getline(line, 512);
		stringList.push_back(line);

	}
	stringList.pop_back();
	stream.close();

	return stringList;
}

DirectoryHandler::~DirectoryHandler()
{
	
}
