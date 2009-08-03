#include <xiot/X3DFIEncodingAlgorithms.h>

#include <iostream>
#include <zlib.h>

#include <xiot/FITypes.h>
#include <xiot/FIConstants.h>
#include <xiot/X3DParseException.h>


#include <xiot/X3DFICompressionTools.h>

namespace XIOT {

	std::string QuantizedzlibFloatArrayAlgorithm::decodeToString(const FI::NonEmptyOctetString &octets) const
	{
		std::vector<float> floatArray = QuantizedzlibFloatArrayAlgorithm::decodeToFloatArray(octets);
		std::stringstream ss;
		std::vector<float>::const_iterator I; 
		for(I = floatArray.begin(); I != floatArray.end() -1; I++)
		{
			ss << (*I) << " ";
		}
		ss << (*I);
		return ss.str();
	}

	std::vector<float> QuantizedzlibFloatArrayAlgorithm::decodeToFloatArray(const FI::NonEmptyOctetString &octets)
	{
		unsigned char exponent = octets[0];
		unsigned char mantissa = octets[1];

		const char* pStr = &octets.front();

		unsigned int len = FI::Tools::readUInt((unsigned char*)pStr+2);
		unsigned int numFloats = FI::Tools::readUInt((unsigned char*)pStr+6);

		int numBits = exponent + mantissa + 1;

		std::vector<Bytef> temp_result(len);

		//std::cout << "Start size: " << octets.size() << std::endl;
		//std::cout << "desBuf size: " << temp_result.size() << std::endl;

		uLong destSize = static_cast<uLong>(temp_result.size());
		int result_code = uncompress(&temp_result.front(), &destSize, (unsigned char*)pStr + 10, static_cast<uLong>(octets.size())-10);
		if (result_code != Z_OK)
			throw new X3DParseException("Error while decoding QuantizedzlibFloatArray");

		std::vector<float> result(numFloats);

		FITools::BitUnpacker bu(&temp_result.front(), destSize);
		FITools::FloatPacker fp(exponent, mantissa);
		for(unsigned int i=0; i < numFloats; i++) {
			unsigned long val = bu.unpack(numBits);
			result[i] = fp.decode(val);
		}
		return result;
	}

	std::string DeltazlibIntArrayAlgorithm::decodeToString(const FI::NonEmptyOctetString &octets) const
	{
		std::vector<int> intArray = DeltazlibIntArrayAlgorithm::decodeToIntArray(octets);
		std::stringstream ss;
		std::vector<int>::const_iterator I;
		for(I = intArray.begin(); I != intArray.end() -1; I++)
		{
			ss << (*I) << " ";
		}
		ss << (*I);
		return ss.str();
	}

	std::vector<int> DeltazlibIntArrayAlgorithm::decodeToIntArray(const FI::NonEmptyOctetString &octets)
	{
		const char* pStr = &octets.front();

		unsigned int length = FI::Tools::readUInt((unsigned char*)pStr);
		unsigned char span = octets[4];

		std::vector<Bytef> temp_result(length*4);

		uLong destSize = static_cast<uLong>(temp_result.size());
		int result_code = uncompress(&temp_result.front(), &destSize, (unsigned char*)pStr + 5, static_cast<uLong>(octets.size())-5);
		if (result_code != Z_OK)
			throw new X3DParseException("Error while decoding DeltazlibIntArrayAlgorithm");

		std::vector<int> result(length);
		
		Bytef* pRes = &temp_result.front();
		for(unsigned int i = 0; i < length; i++)
		{
			result[i] = FI::Tools::readUInt(pRes) -1;
			if (span && i >= span)
				result[i] += result[i-span];
			pRes+=4;
		}
		return result;
	}
}

