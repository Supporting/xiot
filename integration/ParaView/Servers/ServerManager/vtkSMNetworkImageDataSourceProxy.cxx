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
#include "vtkSMOutputPort.h"

#include <vtksys/SystemTools.hxx>

vtkStandardNewMacro(vtkSMNetworkImageDataSourceProxy);
vtkCxxRevisionMacro(vtkSMNetworkImageDataSourceProxy, "$Revision: 1.8 $");

//----------------------------------------------------------------------------
vtkSMNetworkImageDataSourceProxy::vtkSMNetworkImageDataSourceProxy()
{
  this->SourceProcess = CLIENT;
  this->SetServers(vtkProcessModule::CLIENT_AND_SERVERS);
  this->InputProxy = NULL;
}

//----------------------------------------------------------------------------
vtkSMNetworkImageDataSourceProxy::~vtkSMNetworkImageDataSourceProxy()
{
}


void vtkSMNetworkImageDataSourceProxy::AddInput(unsigned int inputPort,
                                vtkSMSourceProxy *input, 
                                unsigned int outputPort,
                                const char* method)
{
  if (!input)
    {
    return;
    }
  this->CreateVTKObjects();
  // Just save the information here, we will receive the
  // info just on demand.
  this->InputProxy = input;
  this->OutputPort = outputPort;
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::SetSourceProcess(int proc)
{
  if (this->SourceProcess != proc)
    {
    this->SourceProcess = proc;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
/*void vtkSMNetworkImageDataSourceProxy::UpdateVTKObjects(vtkClientServerStream& stream)
{
  this->Superclass::UpdateVTKObjects(stream);
}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::ReviveVTKObjects()
{
  this->Superclass::ReviveVTKObjects();
  // When loading revival state, assume that the image is loaded correctly.
  this->ForceNoUpdates = true;
}*/

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::UpdateImage()
{
  if ((this->SourceProcess & this->Servers) == 0)
    {
    vtkErrorMacro("The proxy VTK objects have not been created on the processes "
      "where the image data is present.");
    return;
    }

  if (!this->InputProxy)
    {
    return;
    }

  vtkWarningMacro(<< "vtkSMNetworkImageDataSourceProxy::UpdateImage" << this->GetServers());

  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  vtkClientServerStream stream;

  stream  << vtkClientServerStream::Invoke
          << this->InputProxy->GetID()
          << "GetOutputDataObject" << this->OutputPort
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << this->GetID()
          << "SetImage"
          << vtkClientServerStream::LastResult
          << vtkClientServerStream::End;
  vtkProcessModule::GetProcessModule()->SendStream(this->ConnectionID, 
    vtkProcessModule::GetRootId(this->SourceProcess),
                 stream);

  int readable = 0;
  if(!pm->GetLastResult(this->ConnectionID,
      vtkProcessModule::GetRootId(this->SourceProcess)).GetArgument(0, 0, &readable) ||
    !readable)
    {
    vtkErrorMacro("Cannot copy image data on the process indicated.");
    return;
    }

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

}

//----------------------------------------------------------------------------
void vtkSMNetworkImageDataSourceProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  
}


