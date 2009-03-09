#include "X3DAttributes.h"

namespace X3D {

bool X3DAttributes::isDEF() const
{
	return getAttributeIndex(X3D::DEF) != ATTRIBUTE_NOT_FOUND;
}

bool X3DAttributes::isUSE() const
{
	return getAttributeIndex(X3D::USE) != ATTRIBUTE_NOT_FOUND;
}

std::string X3DAttributes::getDEF() const
{
	int index = getAttributeIndex(X3D::DEF);
	return index == ATTRIBUTE_NOT_FOUND ? "" : getSFString(index);
}

std::string X3DAttributes::getUSE() const
{
	int index = getAttributeIndex(X3D::USE);
	return index == ATTRIBUTE_NOT_FOUND ? "" : getSFString(index);
}

} // namespace X3D

