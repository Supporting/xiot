#include "X3DXMLAttributes.h"

#include "expat.h"
#include <cstring>

namespace XIOT {

struct ExpatAttribute {
	ExpatAttribute(const char* name, const char* value) : _name(name), _value(value) {};
	const char* _name;
	const char* _value;
};

class XMLAttributeImpl
{
public:
	XMLAttributeImpl(const void *const va) {
		if (va != NULL)
		{
			const char** a1 = (const char**)va;
			while(*a1 != 0)
			{
				const char* attrName = *a1++;
				const char* attrValue = *a1++;
				_attributes.push_back(ExpatAttribute(attrName, attrValue));
			}
		}
	}
	std::vector<ExpatAttribute> _attributes;
};



X3DXMLAttributes::X3DXMLAttributes(const void *const va) : 
_impl(new XMLAttributeImpl(va))
{
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
	
	const char* sAttribute = X3DTypes::getAttributeByID(attributeID);
	std::vector<ExpatAttribute>::iterator I = _impl->_attributes.begin();
	while(I!=_impl->_attributes.end())
	{
		if(!strcmp(sAttribute, (*I)._name))
			return I -  _impl->_attributes.begin();
		I++;
	}
	return -1;
}

size_t X3DXMLAttributes::getLength() const {
	return _impl->_attributes.size();
}

std::string X3DXMLAttributes::getAttributesAsString() const {
	std::string sAttributes;
	std::vector<ExpatAttribute>::iterator I = _impl->_attributes.begin();
	while(I!=_impl->_attributes.end())
	{
		sAttributes += (*I)._name;
		sAttributes += " ";
	}
	return sAttributes;
}
// Single fields
bool X3DXMLAttributes::getSFBool(int index) const{
	const char* sValue = _impl->_attributes.at(index)._value;
	return X3DDataTypeFactory::getSFBoolFromString(sValue);
}

float X3DXMLAttributes::getSFFloat(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	return X3DDataTypeFactory::getSFFloatFromString(sValue);
}

int X3DXMLAttributes::getSFInt32(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	return X3DDataTypeFactory::getSFInt32FromString(sValue);
}

SFVec3f X3DXMLAttributes::getSFVec3f(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	return X3DDataTypeFactory::getSFVec3fFromString(sValue);
}

SFVec2f X3DXMLAttributes::getSFVec2f(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	return X3DDataTypeFactory::getSFVec2fFromString(sValue);
}

SFRotation X3DXMLAttributes::getSFRotation(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	return X3DDataTypeFactory::getSFRotationFromString(sValue);
}
std::string X3DXMLAttributes::getSFString(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	return std::string(sValue);
}

SFColor X3DXMLAttributes::getSFColor(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	SFColor result = X3DDataTypeFactory::getSFColorFromString(sValue);
	return result; 
}

SFColorRGBA X3DXMLAttributes::getSFColorRGBA(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	SFColorRGBA result = X3DDataTypeFactory::getSFColorRGBAFromString(sValue);
	return result; 
}

SFImage X3DXMLAttributes::getSFImage(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	SFImage result = X3DDataTypeFactory::getSFImageFromString(sValue);
	return result; 
} 

// Multi Field
std::vector<float> X3DXMLAttributes::getMFFloat(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<float> result = X3DDataTypeFactory::getMFFloatFromString(sValue);
	return result; 
}
std::vector<int> X3DXMLAttributes::getMFInt32(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<int> result = X3DDataTypeFactory::getMFInt32FromString(sValue);
	return result; 
}

std::vector<SFVec3f> X3DXMLAttributes::getMFVec3f(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<SFVec3f> result = X3DDataTypeFactory::getMFVec3fFromString(sValue);
	return result; 
}
std::vector<SFVec2f> X3DXMLAttributes::getMFVec2f(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<SFVec2f> result = X3DDataTypeFactory::getMFVec2fFromString(sValue);
	return result; 
}
std::vector<SFRotation> X3DXMLAttributes::getMFRotation(int index) const  {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<SFRotation> result = X3DDataTypeFactory::getMFRotationFromString(sValue);
	return result; 
}

std::vector<std::string> X3DXMLAttributes::getMFString(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<std::string> result = X3DDataTypeFactory::getMFStringFromString(sValue);
	return result; 
}

std::vector<SFColor> X3DXMLAttributes::getMFColor(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<SFColor> result = X3DDataTypeFactory::getMFColorFromString(sValue);
	return result; 
}

std::vector<SFColorRGBA> X3DXMLAttributes::getMFColorRGBA(int index) const {
	const char* sValue = _impl->_attributes.at(index)._value;
	std::vector<SFColorRGBA> result = X3DDataTypeFactory::getMFColorRGBAFromString(sValue);
	return result; 
}
}

