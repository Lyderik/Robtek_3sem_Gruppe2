#include "TreeElement.h"



TreeElement::TreeElement()
{
}

TreeElement::TreeElement(std::string Path, std::string Name)
{
	path = Path + "\\" + Name;
	name = Name;
}


TreeElement::~TreeElement()
{
}
