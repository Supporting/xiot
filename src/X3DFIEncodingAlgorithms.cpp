#include <xiot/X3DFIEncodingAlgorithms.h>

#include <iostream>
#include <cmath>

#include "zlib.h"

#include <xiot/FITypes.h>
#include <xiot/FIConstants.h>
#include <xiot/X3DParseException.h>
#include <xiot/X3DFICompressionTools.h>

namespace XIOT {

	std::string QuantizedzlibFloatArrayAlgorithm::decodeToString(const FI::NonEmptyOctetString &octets) const
	{
		std::vector<float> floatArray;
		QuantizedzlibFloatArrayAlgorithm::decodeToFloatArray(octets, floatArray);
		
		std::stringstream ss;
		std::vector<float>::const_iterator I; 
		for(I = floatArray.begin(); I != floatArray.end() -1; I++)
		{
			ss << (*I) << " ";
		}
		ss << (*I);
		return ss.str();
	}

	void QuantizedzlibFloatArrayAlgorithm::decodeToFloatArray(const FI::NonEmptyOctetString &octets, std::vector<float> &vec)
	{
    // The format for encoding the custom float format is : (-S)000EEEE|000MMMMM.
    bool sign = !static_cast<bool>(octets[0] & 0x80);
    unsigned char exponent = octets[0] & FI::Constants::LAST_FOUR_BITS;
		unsigned char mantissa = octets[1]  & FI::Constants::LAST_FIVE_BITS;

    int numBits = exponent + mantissa + (sign ? 1 : 0);

		const unsigned char* pStr = &octets.front();

		unsigned int len = FI::Tools::readUInt(pStr+2);
		unsigned int numFloats = FI::Tools::readUInt(pStr+6); // = (len * 8) / (exponent + mantissa + sign); 

		std::vector<Bytef> temp_result(len);
    uLong sourceLen = static_cast<uLong>(octets.size()-10);
		uLong destSize = static_cast<uLong>(temp_result.size());
		
    int result_code = uncompress(&temp_result.front(), &destSize, (unsigned char*)pStr + 10, sourceLen);
    if (result_code != Z_OK) {
      std::stringstream ss;
      ss << "Error while decoding QuantizedzlibFloatArray. ZLIB error code: " << result_code;
      throw X3DParseException(ss.str());
    }

		std::vector<float> result(numFloats);

		FITools::BitUnpacker bu(&temp_result.front(), destSize);
		FITools::FloatPacker fp(exponent, mantissa);
		for(unsigned int i=0; i < numFloats; i++) {
			unsigned long val = bu.unpack(numBits);
			result[i] = fp.decode(val, sign);
		}
		std::swap(result, vec);
	}

	void QuantizedzlibFloatArrayAlgorithm::encode(const float* values, size_t size, FI::NonEmptyOctetString &octets)
	{
		unsigned char* bytes = new unsigned char[size*4];
		unsigned char* bytepos = bytes;
		size_t i;

		const float* vf = values;
		for (i = 0; i < size; i++)
		{
			union float_to_unsigned_int_to_bytes
			{
				float f;
				unsigned int ui;
				unsigned char ub[4]; // unsigned bytes
			};
			float_to_unsigned_int_to_bytes v;
			v.f = (*vf) * 2.0f;

			// Avoid -0
			if (v.ui == 0x80000000)
			{
				v.f = 0.0f;
			}
			// std::cout << "value: " << v << " bytes: " << (int)s[0] << " " << (int)s[1] << " " << (int)s[2] << " " << (int)s[3]) << std::endl;
			*bytepos++ = v.ub[3];
			*bytepos++ = v.ub[2];
			*bytepos++ = v.ub[1];
			*bytepos++ = v.ub[0];
			vf++;
		}


		// Compress the data
		unsigned long compressedSize = static_cast<unsigned long>(size*4) + static_cast<unsigned long>(ceil((size*4)*0.001)) + 12;
		Bytef* compressedData = new Bytef[compressedSize];

		// Call zlib's compress function.
		if(compress2(compressedData, &compressedSize, reinterpret_cast<const Bytef*>(bytes), static_cast<unsigned long>(size*4), Z_DEFAULT_COMPRESSION) != Z_OK)
		{    
			throw X3DParseException("Error while encoding QuantizedzlibFloatArrayAlgorithm");
		}

		unsigned char *s;
		// Put the number of bits for exponent
    octets.push_back(static_cast<unsigned char>(8));
		// Put the number of bits for mantissa
    octets.push_back(static_cast<unsigned char>(23));
		
		// Put the length
		int length = static_cast<int>(size*4);
		int length_reversed = FI::Tools::reverseBytes(&length);
		s = reinterpret_cast <unsigned char*> (&length_reversed);
    octets.insert(octets.end(), s, s+3);

		// Put the number of floats
		int numFloats = static_cast<int>(size);
		int numFloats_reversed = FI::Tools::reverseBytes(&numFloats);;
		s = reinterpret_cast <unsigned char*> (&numFloats_reversed);
    octets.insert(octets.end(), s, s+3);
    //octets.insert(octets.end(), s[0], s[3]);
		//octets.append(s, 4);

		for (i = 0; i < compressedSize; i++)
		{
			unsigned char c = compressedData[i];
      octets.push_back(c);
		}
		delete[] compressedData;
		delete[] bytes;
	}

	std::string DeltazlibIntArrayAlgorithm::decodeToString(const FI::NonEmptyOctetString &octets) const
	{
		std::vector<int> intArray;
		DeltazlibIntArrayAlgorithm::decodeToIntArray(octets, intArray);

		std::stringstream ss;
		std::vector<int>::const_iterator I;
		for(I = intArray.begin(); I != intArray.end() -1; I++)
		{
			ss << (*I) << " ";
		}
		ss << (*I);
		return ss.str();
	}

	void DeltazlibIntArrayAlgorithm::decodeToIntArray(const FI::NonEmptyOctetString &octets, std::vector<int> &vec)
	{
		const unsigned char* pStr = &octets.front();

		unsigned int length = FI::Tools::readUInt((unsigned char*)pStr);
		unsigned char span = octets[4];

		std::vector<Bytef> temp_result(length*4);
    uLong sourceLen = static_cast<uLong>(octets.size()-5);
		uLong destSize = static_cast<uLong>(temp_result.size());
		const Bytef *source = (const Bytef *)(pStr + 5);

    int result_code = uncompress(&temp_result.front(), &destSize, source, sourceLen);
    if (result_code != Z_OK) {
      std::stringstream ss;
      ss << "Error while decoding DeltazlibIntArrayAlgorithm. ZLIB error code: " << result_code;
      throw X3DParseException(ss.str());
    }
		std::vector<int> result(length);

		Bytef* pRes = &temp_result.front();
		for(unsigned int i = 0; i < length; i++)
		{
			result[i] = FI::Tools::readUInt(pRes) -1;
			if (span && i >= span)
				result[i] += result[i-span];
			pRes+=4;
		}
		std::swap(result, vec);
	}

	void DeltazlibIntArrayAlgorithm::encode(const int* values, size_t size, FI::NonEmptyOctetString &octets, bool isImage)
	{
		// compute delta
		char span = 0;
		size_t i = 0;
		int f; unsigned char *p;
		std::vector<unsigned char> deltas;

		if (isImage)
		{
			span = 0;
			for(i = 0; i < size; i++)
			{
				int v = 1 + (values[i]);
				int *vp = reinterpret_cast<int*>(&v);
				f = FI::Tools::reverseBytes(vp);
				p = reinterpret_cast <unsigned char*> (&f);
				deltas.push_back(p[0]);
				deltas.push_back(p[1]);
				deltas.push_back(p[2]);
				deltas.push_back(p[3]);
			}
		}
		else
		{
			for (i = 0; i < 20; i++)
			{
				if (values[i] == -1)
				{
					span = static_cast<char>(i) + 1;
					break;
				}
			}
			if (!span) span = 4;

			for(i = 0; i < static_cast<size_t>(span); i++)
			{
				int v = 1 + values[i];
				int *vp = reinterpret_cast<int*>(&v);
				f = FI::Tools::reverseBytes(vp);

				p = reinterpret_cast <unsigned char*> (&f);
				deltas.push_back(p[0]);
				deltas.push_back(p[1]);
				deltas.push_back(p[2]);
				deltas.push_back(p[3]);
			}
			for(i = span; i < size; i++)
			{
				int v = 1 + (values[i] - values[i-span]);
				f = FI::Tools::reverseBytes(&v);

				p = reinterpret_cast <unsigned char*> (&f);
				deltas.push_back(p[0]);
				deltas.push_back(p[1]);
				deltas.push_back(p[2]);
				deltas.push_back(p[3]);
			}
		}

		unsigned long compressedSize = static_cast<unsigned long>(deltas.size() + ceil(deltas.size()*0.001)) + 12;
		Bytef* compressedData = new Bytef[compressedSize];

		// Call zlib's compress function.
		if(compress2(compressedData, &compressedSize, reinterpret_cast<const Bytef*>(&deltas[0]), static_cast<unsigned long>(deltas.size()), Z_DEFAULT_COMPRESSION) != Z_OK)
		{    
			throw X3DParseException("Error while encoding DeltazlibIntArrayAlgorithm");
		}

		int size32 = static_cast<int>(size);
		int size32_reversed = FI::Tools::reverseBytes(&size32);
		char *s = reinterpret_cast <char*> (&size32_reversed);

		octets.insert(octets.begin(), s, s+4);
		octets.push_back(span);

		for (i = 0; i < compressedSize; i++)
		{
			octets.push_back(compressedData[i]);
		}
		delete[] compressedData;

	}
}

