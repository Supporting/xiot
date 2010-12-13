#include <xiot/X3DXMLAttributes.h>
#include <qxml.h>

#define GET_ATTR_VAL_STR(i) _impl->_attributes->value((i)).toAscii().constData()

namespace XIOT {

class XMLAttributeImpl
{
public:
	QXmlAttributes *_attributes;
};

X3DXMLAttributes::X3DXMLAttributes(const void *const attributes) : 
_impl(new XMLAttributeImpl)
{
	_impl->_attributes = (QXmlAttributes *)attributes;
}

X3DXMLAttributes::~X3DXMLAttributes()
{
  delete _impl;
}


/*!
 * <b>getAttributeIndex</b> returns the attribute's index while it takes the attribute's ID.
 * 
 * @param attributeID The id of the node as specified in 
 * @link{http://www.web3d.org/x3d/specifications/ISO-IEC-FCD-19776-3.2-X3DEncodings-CompressedBinary/Part03/tables.html}
 */
int X3DXMLAttributes::getAttributeIndex(int attributeID) const{
	
	QString localName = X3DTypes::getAttributeByID(attributeID);
	return _impl->_attributes->index(localName);
}

size_t X3DXMLAttributes::getLength() const {
	return _impl->_attributes->length();
}

std::string X3DXMLAttributes::getAttributeValue(int id) const {
  return _impl->_attributes->value(id).toAscii().constData(); 
}

std::string X3DXMLAttributes::getAttributeName(int id) const {
	QString qualifiedName = _impl->_attributes->qName(id);
  return qualifiedName.toAscii().constData(); 
}

// Single fields
bool X3DXMLAttributes::getSFBool(int index) const{
	return X3DDataTypeFactory::getSFBoolFromString(GET_ATTR_VAL_STR(index));
}

float X3DXMLAttributes::getSFFloat(int index) const {
	return X3DDataTypeFactory::getSFFloatFromString(GET_ATTR_VAL_STR(index));
}
int X3DXMLAttributes::getSFInt32(int index) const {
	return X3DDataTypeFactory::getSFInt32FromString(GET_ATTR_VAL_STR(index));
}

void X3DXMLAttributes::getSFVec3f(int index, SFVec3f &value) const {
	X3DDataTypeFactory::getSFVec3fFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getSFVec2f(int index, SFVec2f &value) const {
	X3DDataTypeFactory::getSFVec2fFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getSFRotation(int index, SFRotation &value) const {
	X3DDataTypeFactory::getSFRotationFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getSFString(int index, SFString &value) const {
	value.assign(GET_ATTR_VAL_STR(index));
}

void X3DXMLAttributes::getSFColor(int index, SFColor &value) const {
	X3DDataTypeFactory::getSFColorFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getSFColorRGBA(int index, SFColorRGBA &value) const {
	X3DDataTypeFactory::getSFColorRGBAFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getSFImage(int index, SFImage &value) const {
	X3DDataTypeFactory::getSFImageFromString(GET_ATTR_VAL_STR(index), value);
} 

// Multi Field
void X3DXMLAttributes::getMFFloat(int index, MFFloat &value) const {
	X3DDataTypeFactory::getMFFloatFromString(GET_ATTR_VAL_STR(index), value);
}
void X3DXMLAttributes::getMFInt32(int index, MFInt32 &value) const {
	X3DDataTypeFactory::getMFInt32FromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getMFVec3f(int index, MFVec3f &value) const {
	X3DDataTypeFactory::getMFVec3fFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getMFVec2f(int index, MFVec2f &value) const {
	X3DDataTypeFactory::getMFVec2fFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getMFRotation(int index, MFRotation &value) const  {
	X3DDataTypeFactory::getMFRotationFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getMFString(int index, MFString &value) const {
	X3DDataTypeFactory::getMFStringFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getMFColor(int index, MFColor &value) const {
	X3DDataTypeFactory::getMFColorFromString(GET_ATTR_VAL_STR(index), value);
}

void X3DXMLAttributes::getMFColorRGBA(int index, MFColorRGBA &value) const {
	X3DDataTypeFactory::getMFColorRGBAFromString(GET_ATTR_VAL_STR(index), value);
}
}

