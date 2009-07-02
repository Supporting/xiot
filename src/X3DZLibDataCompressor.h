/*=========================================================================
     This file is part of the XIOT library.

     Copyright (C) 2008-2009 EDF R&D
     Author: Kristian Sons (xiot@actor3d.com)

     This library is free software; you can redistribute it and/or modify
     it under the terms of the GNU Lesser Public License as published by
     the Free Software Foundation; either version 2.1 of the License, or
     (at your option) any later version.

     The XIOT library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser Public License for more details.

     You should have received a copy of the GNU Lesser Public License
     along with XIOT; if not, write to the Free Software
     Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
     MA 02110-1301  USA
=========================================================================*/
#ifndef X3DZLibDataCompressor_H
#define X3DZLibDataCompressor_H

class X3DZLibDataCompressor
{
public:

  X3DZLibDataCompressor();
  ~X3DZLibDataCompressor();
  
  // Description:  
  // Get the maximum space that may be needed to store data of the
  // given uncompressed size after compression.  This is the minimum
  // size of the output buffer that can be passed to the four-argument
  // Compress method.
  unsigned long getMaximumCompressionSpace(unsigned long size);

  // Description:
  // Get/Set the compression level.
  void setCompressionLevel(int n_CompressionLevel);
  int  getCompressionLevel();

public:
  
  
  int _compressionLevel;

  // Wrapping for compression method of zlib
  unsigned long compressBuffer(const unsigned char* uncompressedData,
                               unsigned long uncompressedSize,
                               unsigned char* compressedData,
                               unsigned long compressionSpace);
  
  // Wrapping for decompression method of zlib
  unsigned long uncompressBuffer(const unsigned char* compressedData,
                                 unsigned long compressedSize,
                                 unsigned char* uncompressedData,
                                 unsigned long uncompressedSize);
};

#endif
