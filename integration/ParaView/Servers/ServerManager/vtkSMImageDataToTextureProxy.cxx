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

#include "vtkObjectFactory.h"
#include "vtkSMNetworkImageDataSourceProxy.h"
#include "vtkSMInputProperty.h"
#include "vtkAlgorithm.h"
#include "vtkClientServerStream.h"
#include "vtkImageData.h"

vtkStandardNewMacro(vtkSMImageDataToTextureProxy);
vtkCxxRevisionMacro(vtkSMImageDataToTextureProxy, "$Revision: 1.2 $");
vtkCxxSetObjectMacro(vtkSMImageDataToTextureProxy,Copier,vtkSMNetworkImageDataSourceProxy);

//----------------------------------------------------------------------------
vtkSMImageDataToTextureProxy::vtkSMImageDataToTextureProxy()
{
  this->SetServers(vtkProcessModule::CLIENT |
    vtkProcessModule::RENDER_SERVER);
  this->Copier = NULL;
}

//----------------------------------------------------------------------------
vtkSMImageDataToTextureProxy::~vtkSMImageDataToTextureProxy()
{
  this->SetCopier(0);
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
  this->CreateVTKObjects();
 
  // This proxy delegates it's input to the input of the
  // copier sub-proxy
  vtkSMInputProperty* inputProp = vtkSMInputProperty::SafeDownCast(this->Copier->GetProperty("Input"));
  if (inputProp->GetNumberOfProxies() == 0 || inputProp->GetProxy(0) != input)
    inputProp->SetInputConnection(0, input, outputPort);
}


void vtkSMImageDataToTextureProxy::UpdateImage()
{
  this->Copier->UpdateImage();
}

//----------------------------------------------------------------------------
void vtkSMImageDataToTextureProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }

  this->SetCopier(vtkSMNetworkImageDataSourceProxy::SafeDownCast(this->GetSubProxy("Source")));
  this->Copier->SetServers(vtkProcessModule::CLIENT_AND_SERVERS);

  this->Superclass::CreateVTKObjects();
  
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


