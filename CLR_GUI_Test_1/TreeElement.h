#pragma once
#include <string>
class TreeElement
{
public:
	TreeElement();
	TreeElement(std::string Path, std::string Name);
	void addToPath();
	~TreeElement();
	std::string path;
	std::string name;
};

