/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkNetworkImageDataSource.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkNetworkImageDataSource - an image data
// source that whose data is encoded in a vtkClientServerStream
// .SECTION Description
// vtkNetworkImageDataSource is a subclass of vtkImageAlgorithm that takes a
// vtkClientServerStream with a message whose only argument is a string
// containing a .vtk dataset for a vtkImageData. Because the string contains
// binary (non-ASCII) data, it is not NULL-terminated, and so the method
// ReadImageFromString was not CSS-wrapped properly if we passed the string
// directly to this method. Instead we pass in the vtkClientServerStream
// and unpack it inside this method. Once the CSS has been unpacked, we
// pass the string to a vtkStructuredPointsReader to read the dataset
// contained in the string.

#ifndef __vtkNetworkImageDataSource_h
#define __vtkNetworkImageDataSource_h

#include "vtkImageAlgorithm.h"

class vtkImageData;
class vtkClientServerStream;

class VTK_EXPORT vtkNetworkImageDataSource : public vtkImageAlgorithm
{
public:
  static vtkNetworkImageDataSource* New();
  vtkTypeRevisionMacro(vtkNetworkImageDataSource, vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Read the image from a file.
  int SetImage(vtkImageData* image); 

  int GetDataDimension();

  // Description:
  // Returns the image data as a string.
  const vtkClientServerStream& GetImageAsString();

  // Description:
  // Pass in a vtkClientServerStream containing a string that is a binary-
  // encoded .vtk dataset containing a vtkImageData.
  void ReadImageFromString(vtkClientServerStream &css);

  // Description:
  // Clears extra internal buffers. Note this invalidates the value returned by
  // GetImageAsString().
  void ClearBuffers();
  
protected:
  vtkNetworkImageDataSource();
  ~vtkNetworkImageDataSource();

  vtkImageData* Buffer;
  vtkClientServerStream* Reply;

  int RequestData(vtkInformation *request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);
  int RequestInformation(vtkInformation *request,
                         vtkInformationVector** inputVector,
                         vtkInformationVector* outputVector);

private:
  vtkNetworkImageDataSource(const vtkNetworkImageDataSource&); // Not implemented.
  void operator=(const vtkNetworkImageDataSource&); // Not implemented.
};

#endif
