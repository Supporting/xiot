#include "X3DXMLAttributes.h"
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace XIOT {

class XMLAttributeImpl
{
public:
	XERCES_CPP_NAMESPACE_QUALIFIER Attributes *_attributes;
};

X3DXMLAttributes::X3DXMLAttributes(const void *const attributes) : 
_impl(new XMLAttributeImpl)
{
	_impl->_attributes = (XERCES_CPP_NAMESPACE_QUALIFIER Attributes *)attributes;
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
	XMLCh *xmlChar = XMLString::transcode(sAttribute);
	int result = _impl->_attributes->getIndex(xmlChar);
	XMLString::release(&xmlChar);
	return result;
}

size_t X3DXMLAttributes::getLength() const {
	return static_cast<size_t>(_impl->_attributes->getLength());
}

std::string X3DXMLAttributes::getAttributesAsString() const {
	std::string sAttributes;

	for(size_t i = 0; i < _impl->_attributes->getLength(); i++) {
		sAttributes += XMLString::transcode(_impl->_attributes->getQName(i));
		sAttributes += " ";
	}

	return sAttributes;
}
// Single fields
bool X3DXMLAttributes::getSFBool(int index) const{
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	bool result = X3DDataTypeFactory::getSFBoolFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

float X3DXMLAttributes::getSFFloat(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	float result = X3DDataTypeFactory::getSFFloatFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}
int X3DXMLAttributes::getSFInt32(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	int result = X3DDataTypeFactory::getSFInt32FromString(sValue);
	XMLString::release(&sValue);
	return result; 

}

SFVec3f X3DXMLAttributes::getSFVec3f(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	SFVec3f result = X3DDataTypeFactory::getSFVec3fFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

SFVec2f X3DXMLAttributes::getSFVec2f(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	SFVec2f result = X3DDataTypeFactory::getSFVec2fFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

SFRotation X3DXMLAttributes::getSFRotation(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	SFRotation result = X3DDataTypeFactory::getSFRotationFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}
std::string X3DXMLAttributes::getSFString(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::string result(sValue);
	XMLString::release(&sValue);
	return result; 
}

SFColor X3DXMLAttributes::getSFColor(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	SFColor result = X3DDataTypeFactory::getSFColorFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

SFColorRGBA X3DXMLAttributes::getSFColorRGBA(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	SFColorRGBA result = X3DDataTypeFactory::getSFColorRGBAFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

SFImage X3DXMLAttributes::getSFImage(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	SFImage result = X3DDataTypeFactory::getSFImageFromString(sValue);
	XMLString::release(&sValue);
	return result; 
} 

// Multi Field
std::vector<float> X3DXMLAttributes::getMFFloat(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<float> result = X3DDataTypeFactory::getMFFloatFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}
std::vector<int> X3DXMLAttributes::getMFInt32(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<int> result = X3DDataTypeFactory::getMFInt32FromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

std::vector<SFVec3f> X3DXMLAttributes::getMFVec3f(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<SFVec3f> result = X3DDataTypeFactory::getMFVec3fFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}
std::vector<SFVec2f> X3DXMLAttributes::getMFVec2f(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<SFVec2f> result = X3DDataTypeFactory::getMFVec2fFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}
std::vector<SFRotation> X3DXMLAttributes::getMFRotation(int index) const  {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<SFRotation> result = X3DDataTypeFactory::getMFRotationFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

std::vector<std::string> X3DXMLAttributes::getMFString(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<std::string> result = X3DDataTypeFactory::getMFStringFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

std::vector<SFColor> X3DXMLAttributes::getMFColor(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<SFColor> result = X3DDataTypeFactory::getMFColorFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}

std::vector<SFColorRGBA> X3DXMLAttributes::getMFColorRGBA(int index) const {
	char* sValue = XMLString::transcode(_impl->_attributes->getValue(index));
	std::vector<SFColorRGBA> result = X3DDataTypeFactory::getMFColorRGBAFromString(sValue);
	XMLString::release(&sValue);
	return result; 
}
}

