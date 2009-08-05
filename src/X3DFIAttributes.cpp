#include <xiot/X3DFIAttributes.h>

#include <iostream>

#include <xiot/FITypes.h>
#include <xiot/FIConstants.h>
#include <xiot/X3DParserVocabulary.h>
#include <xiot/X3DParseException.h>
#include <xiot/X3DFIEncodingAlgorithms.h>
#include <xiot/X3DFICompressionTools.h>

#define getValueAt(index) _impl->_attributes->at((index))._normalizedValue

namespace XIOT {

class FIAttributeImpl
{
public:
	FI::Attributes* _attributes;
	FI::ParserVocabulary* _vocab;
};

X3DFIAttributes::X3DFIAttributes(const void *const attributes, const FI::ParserVocabulary* vocab) : 
_impl(new FIAttributeImpl())
{
	_impl->_attributes = (FI::Attributes*) attributes;
	_impl->_vocab = (FI::ParserVocabulary*) vocab;
}

X3DFIAttributes::~X3DFIAttributes()
{
	delete _impl;
}


int X3DFIAttributes::getAttributeIndex(int attributeID) const{
	int i = 0;
	for(std::vector<FI::Attribute>::const_iterator I = _impl->_attributes->begin();
		I != _impl->_attributes->end(); I++, i++)
	{
		if ((*I)._qualifiedName._nameSurrogateIndex == static_cast<unsigned int>(attributeID+1))
			return i;
	}
	return -1;
}

size_t X3DFIAttributes::getLength() const {
	return _impl->_attributes->size();
}

std::string X3DFIAttributes::getAttributesAsString() const {
	std::string sAttributes;
	
	for(size_t i = 0; i < getLength(); i++) {
		sAttributes += X3DTypes::getAttributeByID(_impl->_attributes->at(i)._qualifiedName._nameSurrogateIndex -1);
		sAttributes += " ";
	}
	return sAttributes;
}
// Single fields
bool X3DFIAttributes::getSFBool(int index) const{
	FI::NonIdentifyingStringOrIndex value = getValueAt(index);
	if(value._stringIndex == X3DParserVocabulary::ATTRIBUT_VALUE_TRUE_INDEX)
		return true;
	if(value._stringIndex == X3DParserVocabulary::ATTRIBUT_VALUE_FALSE_INDEX)
		return false;
	if(value._stringIndex == FI::INDEX_NOT_SET)
		return X3DDataTypeFactory::getSFBoolFromString(_impl->_vocab->resolveAttributeValue(value)); 
	
	throw new X3DParseException("Unknown SFBool encoding");
}

float X3DFIAttributes::getSFFloat(int index) const {
	std::vector<float> result = getFloatArray(getValueAt(index));
	if (result.size() == 1)
	{
		return result[0];
	}
	else
		throw new X3DParseException("Wrong size for SFFloat");
}
int X3DFIAttributes::getSFInt32(int index) const {
	std::vector<int> result = getIntArray(getValueAt(index));
	if (result.size() == 1)
	{
		return result[0];
	}
	else
		throw new X3DParseException("Wrong size for SFInt32");
}

SFVec3f X3DFIAttributes::getSFVec3f(int index) const {
	std::vector<float> result = getFloatArray(getValueAt(index));
	if (result.size() == 3)
	{
		SFVec3f c(&result.front());
		return c;
	}
	else
		throw new X3DParseException("Wrong size for SFVec3f");
}

SFVec2f X3DFIAttributes::getSFVec2f(int index) const {
	std::vector<float> result = getFloatArray(getValueAt(index));
	if (result.size() == 2)
	{
		SFVec2f c(&result.front());
		return c;
	}
	else
		throw new X3DParseException("Wrong size for SFVec2f");
}

SFRotation X3DFIAttributes::getSFRotation(int index) const {
	std::vector<float> result = getFloatArray(getValueAt(index));
	if (result.size() == 4)
	{
		SFRotation c(&result.front());
		return c;
	}
	else
		throw new X3DParseException("Wrong size for SFRotation");
}

std::string X3DFIAttributes::getSFString(int index) const {
	return _impl->_vocab->resolveAttributeValue(getValueAt(index)); 
}

SFColor X3DFIAttributes::getSFColor(int index) const {
	std::vector<float> result = getFloatArray(getValueAt(index));
	if (result.size() == 3)
	{
		SFColor c(&result.front());
		return c;
	}
	else
		throw new X3DParseException("Wrong size for SFColor");
}



SFColorRGBA X3DFIAttributes::getSFColorRGBA(int index) const {
	std::vector<float> result = getFloatArray(getValueAt(index));
	if (result.size() == 4)
	{
		SFColorRGBA c(&result.front());
		return c;
	}
	else
		throw new X3DParseException("Wrong size for SFColorRGBA");
}

SFImage X3DFIAttributes::getSFImage(int index) const {
	return getIntArray(getValueAt(index));
} 

// Multi Field
std::vector<float> X3DFIAttributes::getMFFloat(int index) const {
	return getFloatArray(getValueAt(index));
}
std::vector<int> X3DFIAttributes::getMFInt32(int index) const {
	return getIntArray(getValueAt(index));
}

std::vector<SFVec3f> X3DFIAttributes::getMFVec3f(int index) const {
	std::vector<float> fArray = getFloatArray(getValueAt(index));
	if (fArray.size() % 3 == 0)
	{
		float* pPos = &fArray.front();
		std::vector<SFVec3f> result;
		for(size_t i = 0; i < fArray.size() / 3; i++, pPos+=3)
		{
			SFVec3f c(pPos);
			result.push_back(c);
		}
		return result;
	}
	else
		throw new X3DParseException("Wrong size for MFVec3f");
}

std::vector<SFVec2f> X3DFIAttributes::getMFVec2f(int index) const {
	std::vector<float> fArray = getFloatArray(getValueAt(index));
	if (fArray.size() % 2 == 0)
	{
		float* pPos = &fArray.front();
		std::vector<SFVec2f> result;
		for(size_t i = 0; i < fArray.size() / 2; i++, pPos+=2)
		{
			SFVec2f c(pPos);
			result.push_back(c);
		}
		return result;
	}
	else
		throw new X3DParseException("Wrong size for MFVec2f");
}

std::vector<SFRotation> X3DFIAttributes::getMFRotation(int index) const  {
	std::vector<float> fArray = getFloatArray(getValueAt(index));
	if (fArray.size() % 4 == 0)
	{
		float* pPos = &fArray.front();
		std::vector<SFRotation> result;
		for(size_t i = 0; i < fArray.size() / 4; i++, pPos+=4)
		{
			SFRotation c(pPos);
			result.push_back(c);
		}
		return result;
	}
	else
		throw new X3DParseException("Wrong size for MFRotation");
}

std::vector<std::string> X3DFIAttributes::getMFString(int index) const {
	return X3DDataTypeFactory::getMFStringFromString(_impl->_vocab->resolveAttributeValue(getValueAt(index))); 
}

std::vector<SFColor> X3DFIAttributes::getMFColor(int index) const {
	std::vector<float> fArray = getFloatArray(getValueAt(index));
	if (fArray.size() % 3 == 0)
	{
		float* pPos = &fArray.front();
		std::vector<SFColor> result;
		for(size_t i = 0; i < fArray.size() / 3; i++, pPos+=3)
		{
			SFColor c(pPos);
			result.push_back(c);
		}
		return result;
	}
	else
		throw new X3DParseException("Wrong size for MFColor");
}

std::vector<SFColorRGBA> X3DFIAttributes::getMFColorRGBA(int index) const {
	std::vector<float> fArray = getFloatArray(getValueAt(index));
	if (fArray.size() % 4 == 0)
	{
		float* pPos = &fArray.front();
		std::vector<SFColorRGBA> result;
		for(size_t i = 0; i < fArray.size() / 4; i++, pPos+=4)
		{
			SFColorRGBA c(pPos);
			result.push_back(c);
		}
		return result;
	}
	else
		throw new X3DParseException("Wrong size for MFColorRGBA");
}

std::vector<float> X3DFIAttributes::getFloatArray(const FI::NonIdentifyingStringOrIndex &value) const
{
	if (value._stringIndex == FI::INDEX_NOT_SET
		&& 	value._characterString._encodingFormat == FI::ENCODINGFORMAT_ENCODING_ALGORITHM)
	{
		switch (value._characterString._encodingAlgorithm)
		{
			case QuantizedzlibFloatArrayAlgorithm::ALGORITHM_ID:
				return QuantizedzlibFloatArrayAlgorithm::decodeToFloatArray(value._characterString._octets);
			case FI::FloatEncodingAlgorithm::ALGORITHM_ID:
				return FI::FloatEncodingAlgorithm::decodeToFloatArray(value._characterString._octets);
		}
	}
	// This is the fallback
	return X3DDataTypeFactory::getMFFloatFromString(_impl->_vocab->resolveAttributeValue(value));
}

std::vector<int> X3DFIAttributes::getIntArray(const FI::NonIdentifyingStringOrIndex &value) const
{
	if (value._stringIndex == FI::INDEX_NOT_SET
		&& 	value._characterString._encodingFormat == FI::ENCODINGFORMAT_ENCODING_ALGORITHM)
	{
		switch(value._characterString._encodingAlgorithm)
		{
			case DeltazlibIntArrayAlgorithm::ALGORITHM_ID:
				return DeltazlibIntArrayAlgorithm::decodeToIntArray(value._characterString._octets);
			case FI::IntEncodingAlgorithm::ALGORITHM_ID:
				return FI::IntEncodingAlgorithm::decodeToIntArray(value._characterString._octets);

		}
	}
	// This is the fallback
	return X3DDataTypeFactory::getMFInt32FromString(_impl->_vocab->resolveAttributeValue(value));
}


}

