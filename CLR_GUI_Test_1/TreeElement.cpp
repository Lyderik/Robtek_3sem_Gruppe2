#include "TreeElement.h"



TreeElement::TreeElement()
{
}

TreeElement::TreeElement(std::string Path, std::string Name)
{
	if (Path != "")
	{
		path = Path + "\\" + Name;
	}
	else
	{
		path = Name;
	}
	name = Name;
}


TreeElement::~TreeElement()
{
}
