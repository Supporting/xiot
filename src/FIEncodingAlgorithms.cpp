#include "FIEncodingAlgorithms.h"

#include <iostream>
#include <cassert>

#include "FITypes.h"
#include "FIConstants.h"


namespace FI {


	std::string FloatEncodingAlgorithm::decodeToString(const FI::NonEmptyOctetString &octets) const
	{
		std::vector<float> floatArray = decodeToFloatArray(octets);
		std::stringstream ss;
		std::vector<float>::const_iterator I; 
		for(I = floatArray.begin(); I != floatArray.end() -1; I++)
		{
			ss << (*I) << " ";
		}
		ss << (*I);
		return ss.str();
	}

	std::vector<float> FloatEncodingAlgorithm::decodeToFloatArray(const FI::NonEmptyOctetString &octets)
	{
		assert(octets.size() % 4 == 0);
		size_t length = octets.size() / 4;
		
		const char*pOctets = &octets.front();
		std::vector<float> result(length);
		for (size_t i = 0; i < length; i++)
		{
			result[i] = Tools::readFloat((const unsigned char*)pOctets);
			pOctets += 4;
		}
		return result;
	}

	std::string IntEncodingAlgorithm::decodeToString(const FI::NonEmptyOctetString &octets) const
	{
		std::vector<int> intArray = decodeToIntArray(octets);
		std::stringstream ss;
		std::vector<int>::const_iterator I;
		for(I = intArray.begin(); I != intArray.end() -1; I++)
		{
			ss << (*I) << " ";
		}
		ss << (*I);
		return ss.str();
	}

	std::vector<int> IntEncodingAlgorithm::decodeToIntArray(const FI::NonEmptyOctetString &octets)
	{
		assert(octets.size() % 4 == 0);
		size_t length = octets.size() / 4;
		
		const char*pOctets = &octets.front();
		std::vector<int> result(length);
		for (size_t i = 0; i < length; i++)
		{
			result[i] = Tools::readUInt((const unsigned char*)pOctets);
			pOctets += 4;
		}
		return result;
	}

std::string BooleanEncodingAlgorithm::decodeToString(const FI::NonEmptyOctetString &octets) const
{
	throw std::runtime_error("BooleanEncodingAlgorithm not implemented (yet)");
	/*std::vector<bool> floatArray = decodeToBoolArray(octets);
	std::stringstream ss;
	std::vector<bool>::const_iterator I; 
	for(I = floatArray.begin(); I != floatArray.end() -1; I++)
	{
		ss << (*I) << " ";
	}
	ss << (*I);
	return ss.str();*/
}

std::vector<bool> BooleanEncodingAlgorithm::decodeToBoolArray(const FI::NonEmptyOctetString &octets)
{
	std::vector<bool> result;
	unsigned char unusedBits = octets[0] >> 4;

	return result;
}

}

