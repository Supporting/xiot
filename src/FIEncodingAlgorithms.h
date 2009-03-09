#ifndef FI_FIENCODINGALGORITHMS_H
#define FI_FIENCODINGALGORITHMS_H

#include "FITypes.h"
#include <string>

namespace FI {

/**
 * Interface that all algorithms need to implement
 */
class OPENFI_EXPORT IEncodingAlgorithm {
	public:
		/**
		* All algorithms have to implement this method to decode the encoded
		* byte array to it´s XML string representation
		*
		* @octets The byte array containing the encoded data
		* @return The string representation of the decoded input
		*/
		virtual std::string decodeToString(const NonEmptyOctetString &octets) const = 0;
};

/**
 * 10.5 The "int" encoding algorithm
 */
class OPENFI_EXPORT IntEncodingAlgorithm : public IEncodingAlgorithm
{
public:
	/// This encoding algorithm has a vocabulary table index of 4
	static const int ALGORITHM_ID = 4;
	virtual std::string decodeToString(const FI::NonEmptyOctetString &octets) const;
	static std::vector<int> decodeToIntArray(const FI::NonEmptyOctetString &octets);
};

/**
 * 10.7 The "boolean" encoding algorithm
 * @warning Not implemented yet
 */
class OPENFI_EXPORT BooleanEncodingAlgorithm : public IEncodingAlgorithm
{
public:
	/// This encoding algorithm has a vocabulary table index of 6
	static const int ALGORITHM_ID = 6;

	virtual std::string decodeToString(const FI::NonEmptyOctetString &octets) const;
	static std::vector<bool> decodeToBoolArray(const FI::NonEmptyOctetString &octets);
};

/**
 * 10.8 The "float" encoding algorithm
 * 
 * @warning Not implemented yet
 */
class OPENFI_EXPORT FloatEncodingAlgorithm : public IEncodingAlgorithm
{
public:
	/// This encoding algorithm has a vocabulary table index of 7
	static const int ALGORITHM_ID = 7;
	virtual std::string decodeToString(const FI::NonEmptyOctetString &octets) const;
	static std::vector<float> decodeToFloatArray(const FI::NonEmptyOctetString &octets);
	
};


/**
 * Some helpers to convert bytes into other datatypes
 */
class Tools {
public:
	union float_to_unsigned_int_to_bytes
		{
			float f;
			unsigned int ui;
			unsigned char ub[4]; // unsigned bytes
		};

	/// converts the next 4 bytes of the input pointer to one 4 byte 
	/// unsigned int swapping the positions of the bytes
	inline static unsigned int readUInt(const unsigned char* bytes)
	{
		float_to_unsigned_int_to_bytes v;
		v.ub[0] = bytes[3];
		v.ub[1] = bytes[2];
		v.ub[2] = bytes[1];
		v.ub[3] = bytes[0];
		return v.ui;
	};

	/// converts the next 4 bytes of the input pointer to one 4 byte float
	/// swapping the positions of the bytes
	inline static float readFloat(const unsigned char* bytes)
	{
		float_to_unsigned_int_to_bytes v;
		v.ub[0] = bytes[3];
		v.ub[1] = bytes[2];
		v.ub[2] = bytes[1];
		v.ub[3] = bytes[0];
		return v.f;
	};

};

}; // End namespace X3D;


#endif

