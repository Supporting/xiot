/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkSMImageDataToTextureProxy.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMImageDataToTextureProxy.h"

#include "vtkAlgorithm.h"
#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkImageData.h"
#include "vtkSMOutputPort.h"
#include "vtkSMNetworkImageDataSourceProxy.h"

vtkStandardNewMacro(vtkSMImageDataToTextureProxy);
vtkCxxRevisionMacro(vtkSMImageDataToTextureProxy, "$Revision: 1.2 $");
//----------------------------------------------------------------------------
vtkSMImageDataToTextureProxy::vtkSMImageDataToTextureProxy()
{
  this->SetServers(vtkProcessModule::CLIENT |
    vtkProcessModule::RENDER_SERVER);
}

//----------------------------------------------------------------------------
vtkSMImageDataToTextureProxy::~vtkSMImageDataToTextureProxy()
{
}

void vtkSMImageDataToTextureProxy::AddInput(unsigned int inputPort,
                                vtkSMSourceProxy *input, 
                                unsigned int outputPort,
                                const char* method)
{
  if (!input)
    {
    return;
    }

  input->CreateOutputPorts();

  this->CreateVTKObjects();
  
  
  vtkClientServerStream stream;
  stream  << vtkClientServerStream::Invoke
          << input->GetID()
          << "GetActiveImage"
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << this->Copier->GetID()
          << "SetImage"
          << vtkClientServerStream::LastResult
          << vtkClientServerStream::End;
  vtkProcessModule::GetProcessModule()->SendStream(this->ConnectionID, 
    input->GetServers() & this->Copier->GetServers(), 
                 stream);

  
  
}

//----------------------------------------------------------------------------
void vtkSMImageDataToTextureProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }

  this->Superclass::CreateVTKObjects();

  this->Copier = this->GetSubProxy("Source");
  this->Copier->SetServers(vtkProcessModule::CLIENT_AND_SERVERS);
  
  vtkClientServerStream stream;
  stream  << vtkClientServerStream::Invoke
          << this->Copier->GetID()
          << "GetOutputPort"
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << this->GetID()
          << "SetInputConnection"
          << vtkClientServerStream::LastResult
          << vtkClientServerStream::End;
  vtkProcessModule::GetProcessModule()->SendStream(this->ConnectionID,
    this->Servers & this->Copier->GetServers(), stream);
}

//----------------------------------------------------------------------------
vtkImageData* vtkSMImageDataToTextureProxy::GetLoadedImage()
{
  vtkSMSourceProxy::SafeDownCast(this->GetSubProxy("Source"))->UpdatePipeline();
  vtkAlgorithm* source = vtkAlgorithm::SafeDownCast(
    this->GetSubProxy("Source")->GetClientSideObject());
  return source? vtkImageData::SafeDownCast(source->GetOutputDataObject(0)): 0;
}

//----------------------------------------------------------------------------
void vtkSMImageDataToTextureProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


