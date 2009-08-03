#include <xiot/X3DZLibDataCompressor.h>
#include "zlib.h"

//----------------------------------------------------------------------------
X3DZLibDataCompressor::X3DZLibDataCompressor()
{
  this->_compressionLevel = Z_DEFAULT_COMPRESSION;
}

//----------------------------------------------------------------------------
X3DZLibDataCompressor::~X3DZLibDataCompressor()
{ 
}


//----------------------------------------------------------------------------
unsigned long
X3DZLibDataCompressor::compressBuffer(const unsigned char* uncompressedData,
                                      unsigned long uncompressedSize,
                                      unsigned char* compressedData,
                                      unsigned long compressionSpace)
{
  unsigned long compressedSize = compressionSpace;
  Bytef* cd = reinterpret_cast<Bytef*>(compressedData);
  const Bytef* ud = reinterpret_cast<const Bytef*>(uncompressedData);
  
  // Call zlib's compress function.
  if(compress2(cd, &compressedSize, ud, uncompressedSize, this->_compressionLevel) != Z_OK)
    {    
		return 0;
    }
  
  return compressedSize;
}

//----------------------------------------------------------------------------
unsigned long
X3DZLibDataCompressor::uncompressBuffer(const unsigned char* compressedData,
                                        unsigned long compressedSize,
                                        unsigned char* uncompressedData,
                                        unsigned long uncompressedSize)
{  
  unsigned long decSize = uncompressedSize;
  Bytef* ud = reinterpret_cast<Bytef*>(uncompressedData);
  const Bytef* cd = reinterpret_cast<const Bytef*>(compressedData);
  
  // Call zlib's uncompress function.
  if(uncompress(ud, &decSize, cd, compressedSize) != Z_OK)
    {    
    return 0;
    }
  
  // Make sure the output size matched that expected.
  if(decSize != uncompressedSize)
    {
    return 0;
    }
  
  return decSize;
}

//----------------------------------------------------------------------------
unsigned long
X3DZLibDataCompressor::getMaximumCompressionSpace(unsigned long size)
{
  // ZLib specifies that destination buffer must be 0.1% larger + 12 bytes.
  return size + (size+999)/1000 + 12;
}

void X3DZLibDataCompressor::setCompressionLevel(int n_CompressionLevel)
{
	if(n_CompressionLevel >= 1 && n_CompressionLevel <= 9)
		_compressionLevel = n_CompressionLevel;
}

int X3DZLibDataCompressor::getCompressionLevel()
{
	return _compressionLevel;
}

