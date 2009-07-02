#include "X3DAttributes.h"

namespace XIOT {

bool X3DAttributes::isDEF() const
{
	return getAttributeIndex(ID::DEF) != ATTRIBUTE_NOT_FOUND;
}

bool X3DAttributes::isUSE() const
{
	return getAttributeIndex(ID::USE) != ATTRIBUTE_NOT_FOUND;
}

std::string X3DAttributes::getDEF() const
{
	int index = getAttributeIndex(ID::DEF);
	return index == ATTRIBUTE_NOT_FOUND ? "" : getSFString(index);
}

std::string X3DAttributes::getUSE() const
{
	int index = getAttributeIndex(ID::USE);
	return index == ATTRIBUTE_NOT_FOUND ? "" : getSFString(index);
}

} // namespace X3D

