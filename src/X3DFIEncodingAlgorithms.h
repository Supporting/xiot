#ifndef X3D_X3DFIENCODINGALGORITHMS_H
#define X3D_X3DFIENCODINGALGORITHMS_H

#include "X3DTypes.h"
#include "FIEncodingAlgorithms.h"
#include <string>

namespace XIOT {

/**
 * Encoding algorithm to encode/decode arrays of type int.
 *
 * This algorithm implements the Delta zlib integer array encoder
 * as described in 5.5.2 in ISO/IEC 19776-3:2007 - Part 3
 *
 * This encoding applies to MFInt32, SFImage and MFImage fields
 *
 * @link(http://www.web3d.org/x3d/specifications/ISO-IEC-19776-3-X3DEncodings-CompressedBinaryEncoding/Part03/EncodingOfFields.html#DeltazlibIntegerArrayEncoder)
 *
 * The URI for identifying this encoder is: "encoder://web3d.org/DeltazlibIntArrayEncoder"
 *
 * @ingroup x3dloader
 */
class X3DLOADER_EXPORT DeltazlibIntArrayAlgorithm : public FI::IEncodingAlgorithm
{
public:
	/// The position of this algorithm in the FI algorithm table.
	static const int ALGORITHM_ID = 34; 

	virtual std::string decodeToString(const FI::NonEmptyOctetString &octets) const;

	/**
	 * Decodes a given vector of unsigned chars to a vector of int using the 
	 * Delta zlib integer array decoder.
	 */
	static std::vector<int> decodeToIntArray(const FI::NonEmptyOctetString &octets);
	
};

/**
 * Encoding algorithm to encode/decode arrays of type float.
 *
 * This algorithm implements the Quantized zlib float array encoder
 * as described in 5.4.3 in ISO/IEC 19776-3:2007 - Part 3
 *
 * @link(http://www.web3d.org/x3d/specifications/ISO-IEC-19776-3-X3DEncodings-CompressedBinaryEncoding/Part03/EncodingOfFields.html#DeltazlibIntegerArrayEncoder)
 *
 * The URI for identifying this encoder is:  "encoder://web3d.org/QuantizedDoubleArrayEncoder"
 *
 * @ingroup x3dloader
 */
class X3DLOADER_EXPORT QuantizedzlibFloatArrayAlgorithm : public FI::IEncodingAlgorithm
{
public:
	/// The position of this algorithm in the FI algorithm table.
	static const int ALGORITHM_ID = 35; 

	virtual std::string decodeToString(const FI::NonEmptyOctetString &octets) const;

	/**
	 * Decodes a given vector of unsigned chars to a vector of float using the 
	 * Quantized zlib float array encoder.
	 */
	static std::vector<float> decodeToFloatArray(const FI::NonEmptyOctetString &octets);
	
};




}; // End namespace X3D;


#endif

