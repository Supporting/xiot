#ifndef X3D_X3DPARSERVOCABULARY_H
#define X3D_X3DPARSERVOCABULARY_H

#include "FIParserVocabulary.h"
#include "X3DLoaderConfigure.h"
#include "X3DFIEncodingAlgorithms.h"
#include <string>
#include <fstream>
#include <map>

namespace XIOT {


/**
 * <b>X3DParserVocabulary</b> implements the External Parser Vocabulary as defined
 * in the X3DB spec.
 *
 * This includes the tables for element and attribute names, for 
 * attribute values and two X3D specific encoding algorithms
 *
 * @link(http://www.web3d.org/x3d/specifications/ISO-IEC-FCD-19776-3.2-X3DEncodings-CompressedBinary/Part03/tables.html)
 *
 * @ingroup x3dloader
 */
class X3DLOADER_EXPORT X3DParserVocabulary : public FI::DefaultParserVocabulary
{
public:
	static const int ATTRIBUT_VALUE_FALSE_INDEX = 1;
	static const int ATTRIBUT_VALUE_TRUE_INDEX = 2;

	X3DParserVocabulary();
	virtual ~X3DParserVocabulary() {};

	QuantizedzlibFloatArrayAlgorithm _quantizedzlibFloatArrayAlgorithm;
	DeltazlibIntArrayAlgorithm _deltazlibIntArrayAlgorithm;
private:
};
}

#endif

