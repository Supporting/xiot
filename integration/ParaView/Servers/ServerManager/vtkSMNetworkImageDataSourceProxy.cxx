/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkSMNetworkImageDataSourceProxy.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMNetworkImageDataSourceProxy.h"

#include "vtkObjectFactory.h"
#include "vtkClientServerStream.h"
#include "vtkImageData.h"

#include <vtksys/SystemTools.hxx>

vtkStandardNewMacro(vtkSMNetworkImageDataSourceProxy);
vtkCxxRevisionMacro(vtkSMNetworkImageDataSourceProxy, "$Revision: 1.8 $");

//----------------------------------------------------------------------------
vtkSMNetworkImageDataSourceProxy::vtkSMNetworkImageDataSourceProxy()
{
  this->Image = NULL;
  this->SourceProcess = CLIENT;
  this->UpdateNeeded = true;
  this->SetServers(vtkProcessModule::CLIENT_AND_SERVERS);
  this->ForceNoUpdates = false;
}

//----------------------------------------------------------------------------
vtkSMNetworkImageDataSourceProxy::~vtkSMNetworkImageDataSourceProxy()
{
  this->SetImage(0);
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::SetImage(vtkImageData* aImage)
{
  if (this->Image == aImage)
    {
    return;
    }
  
  vtkImageData* oldImage = this->Image;
  this->Image = aImage;

  if (this->Image != NULL)
    {
    this->Image->Register(this);
    }
  if (oldImage != NULL)
    {
    oldImage->UnRegister(this);
    }

  this->Modified();
  this->UpdateNeeded = true;
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::SetSourceProcess(int proc)
{
  if (this->SourceProcess != proc)
    {
    this->SourceProcess = proc;
    this->Modified();
    this->UpdateNeeded = true;
    }
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::UpdateVTKObjects(vtkClientServerStream& stream)
{
  this->Superclass::UpdateVTKObjects(stream);
  if (this->UpdateNeeded && !this->ForceNoUpdates)
    {
    this->UpdateImage();
    }
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::ReviveVTKObjects()
{
  this->Superclass::ReviveVTKObjects();
  // When loading revival state, assume that the image is loaded correctly.
  this->ForceNoUpdates = true;
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::UpdateImage()
{
  if ((this->SourceProcess & this->Servers) == 0)
    {
    vtkErrorMacro("The proxy VTK objects have not been created on the processes "
      "where the image data is present.");
    return;
    }

  cout << "UpdateImage" << endl;
  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  vtkClientServerStream stream;
  /*stream  << vtkClientServerStream::Invoke
          << this->GetID() << "SetImage"
          << this->Image 
          << vtkClientServerStream::End;
  pm->SendStream(this->ConnectionID, 
    vtkProcessModule::GetRootId(this->SourceProcess), stream);

  int readable = 0;
  if(!pm->GetLastResult(this->ConnectionID,
      vtkProcessModule::GetRootId(this->SourceProcess)).GetArgument(0, 0, &readable) ||
    !readable)
    {
    vtkErrorMacro("Cannot copy image data on the process indicated.");
    return;
    }*/

  stream  << vtkClientServerStream::Invoke
          << this->GetID() << "GetImageAsString"
          << vtkClientServerStream::End;
  pm->SendStream(this->ConnectionID, 
    vtkProcessModule::GetRootId(this->SourceProcess), stream);

  vtkClientServerStream reply;
  int retVal = pm->GetLastResult(this->ConnectionID,
    vtkProcessModule::GetRootId(this->SourceProcess)).GetArgument(0, 0, &reply);

  stream << vtkClientServerStream::Invoke
         << this->GetID()
         << "ClearBuffers"
         << vtkClientServerStream::End;
  pm->SendStream(this->ConnectionID, this->Servers, stream);

  if(!retVal)
    {
    vtkErrorMacro("Error getting reply from server.");
    return;
    }

  // Now transmit the reply to all the processes where the image is not
  // present.
  stream  << vtkClientServerStream::Invoke
          << this->GetID() << "ReadImageFromString"
          << reply
          << vtkClientServerStream::End;
  reply.Reset();

  pm->SendStream(this->ConnectionID, this->Servers, stream);

  this->UpdateNeeded = false;
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  
  if (this->Image)
    {
    os << indent << "Image: " << this->Image;
    }
  else
    {
    os << indent << "Image: (none)" << endl;
    }
}


