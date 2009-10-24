/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkNetworkImageDataSource.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkObjectFactory.h"

#include "vtkCharArray.h"
#include "vtkClientServerStream.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNetworkImageDataSource.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsReader.h"
#include "vtkJPEGReader.h"
#include "vtkBMPReader.h"
#include "vtkTIFFReader.h"
#include "vtkPNMReader.h"
#include "vtkPNGReader.h"
#include "vtkSmartPointer.h"
#include "vtkStructuredPointsWriter.h"
#include <vtksys/SystemTools.hxx>


vtkStandardNewMacro(vtkNetworkImageDataSource);
vtkCxxRevisionMacro(vtkNetworkImageDataSource, "$Revision: 1.4 $");
//----------------------------------------------------------------------------
vtkNetworkImageDataSource::vtkNetworkImageDataSource()
{
  this->SetNumberOfInputPorts(0);
  this->Buffer = vtkImageData::New();
  this->Reply = new vtkClientServerStream();
}

//----------------------------------------------------------------------------
vtkNetworkImageDataSource::~vtkNetworkImageDataSource()
{
  delete this->Reply;
  this->Buffer->Delete();
}

//----------------------------------------------------------------------------
int vtkNetworkImageDataSource::SetImage(vtkImageData* image)
{
  if (!image)
    {
    vtkErrorMacro("Image must not be null.");
    return 0;
    }

  
  this->Buffer->ShallowCopy(image);
  this->Modified();
  return 1;
}

//----------------------------------------------------------------------------
const vtkClientServerStream& vtkNetworkImageDataSource::GetImageAsString()
{
 // Read the texture image locally and write it out to a binary string.
  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  writer->SetInput(this->Buffer);
  writer->SetFileTypeToBinary();
  writer->WriteToOutputStringOn();
  writer->Write();

  this->Reply->Reset(); 
  (*this->Reply)  << vtkClientServerStream::Reply
                  << vtkClientServerStream::InsertArray(
                    writer->GetOutputString(), writer->GetOutputStringLength())
                  << vtkClientServerStream::End;
  writer->Delete();
  return (*this->Reply);
}

//----------------------------------------------------------------------------
void vtkNetworkImageDataSource::ClearBuffers()
{
  this->Buffer->Initialize();
  delete this->Reply;
  this->Reply = new vtkClientServerStream();
}

//----------------------------------------------------------------------------
void vtkNetworkImageDataSource::ReadImageFromString(vtkClientServerStream& css)
{
  // Get the length of the string in the vtkClientServerStream.
  vtkTypeUInt32 len;
  if (!css.GetArgumentLength(0, 0, &len))
    {
    abort();
    }
  this->ClearBuffers();

  // Get the string (a .vtk dataset containing a vtkImageData) from the
  // vtkClientServerStream.
  char* imageString = new char[len];
  css.GetArgument(0, 0, imageString, len);

  // Set up a vtkCharArray to hold the string. We do it this way rather than
  // pass the string directly to the vtkStructuredPointsReader to avoid
  // duplicating the string in memory.
  vtkCharArray *inputArray = vtkCharArray::New();
  inputArray->SetArray(imageString, len, 1);

  // Read the data from the string.
  vtkStructuredPointsReader *reader = vtkStructuredPointsReader::New();
  reader->SetInputArray(inputArray);
  reader->ReadFromInputStringOn();
  reader->Update();

  // shallow copy the output of the vtkStructuredPointsReader to an
  // internal buffer
  this->Buffer->ShallowCopy(reader->GetOutput());

  inputArray->Delete();
  reader->Delete();
  delete [] imageString;
}

//----------------------------------------------------------------------------
int vtkNetworkImageDataSource::RequestInformation(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               this->Buffer->GetExtent(), 6);

  return 1;
}

//----------------------------------------------------------------------------
int vtkNetworkImageDataSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* outputVector)
{
  // shallow copy internal buffer to output
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *output = vtkImageData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  output->ShallowCopy(this->Buffer);
  return 1;
}

//----------------------------------------------------------------------------
void vtkNetworkImageDataSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
