#include "TreeElement.h"



TreeElement::TreeElement()
{
}

TreeElement::TreeElement(std::string Path, std::string Name, bool combine)
{
	if (combine)
	{
		path = Path + "\\" + Name;
	}
	else
	{
		path = Path;
	}
	name = Name;
}


TreeElement::~TreeElement()
{
}
