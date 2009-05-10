/*=========================================================================

/// This file is part of the X3DExporter library
/// Copyright (C) 2008 Kristian Sons (x3dexporter@actor3d.com)

/// This project has been funded with support from EDF R&D
/// Christophe MOUTON

/// TODO: Link

/// This library is free software; you can redistribute it and/or
/// modify it under the terms of the GNU Lesser General Public
/// License as published by the Free Software Foundation; either
/// version 2.1 of the License, or (at your option) any later version.

/// This library is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/// Lesser General Public License for more details.

/// You should have received a copy of the GNU Lesser General Public
/// License along with this library; if not, write to the Free Software
/// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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
