#include <xiot/X3DFIEncoder.h>
#include <xiot/FIConstants.h>
#include <xiot/FIEncodingAlgorithms.h>
#include <xiot/X3DFIEncodingAlgorithms.h>

using namespace std;

namespace XIOT {

X3DFIEncoder::X3DFIEncoder(void): FIEncoder(), _fastest(true)
{
	reset();
}

X3DFIEncoder::~X3DFIEncoder(void)
{
}

void X3DFIEncoder::setFastest(bool fastest)
{
	_fastest = fastest;
}


void X3DFIEncoder::encodeAttributeFloatArray(const float* values, size_t size)
{
  // We want to start at position 3
  assert(_currentBytePos == 2);

  if (_fastest || size < 15)
  {
	  FIEncoder::encodeAttributeFloatArray(values, size);
	  return;
  }

  encodeEncodingAlgorithmStart(QuantizedzlibFloatArrayAlgorithm::ALGORITHM_ID);

  FI::NonEmptyOctetString octets;
  QuantizedzlibFloatArrayAlgorithm::encode(values, size, octets);
  encodeNonEmptyByteString5(octets);
}

void X3DFIEncoder::encodeAttributeIntegerArray(const int* values, size_t size)
{
  // We want to start at position 3
  assert(_currentBytePos == 2);

  if (size < 15)
  {
	  FIEncoder::encodeAttributeIntegerArray(values, size);
	  return;
  }

  encodeEncodingAlgorithmStart(DeltazlibIntArrayAlgorithm::ALGORITHM_ID);
  
  FI::NonEmptyOctetString octets;
  DeltazlibIntArrayAlgorithm::encode(values, size, octets);
  encodeNonEmptyByteString5(octets);
}

} // namespace FI