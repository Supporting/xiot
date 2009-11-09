/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkSMX3DImporterProxy.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMX3DImporterProxy.h"

#include "vtkAlgorithm.h"
#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMProxyManager.h"
#include "vtkSMProxyProperty.h"
#include "vtkImageData.h"
#include "vtkX3DSource.h"
#include "vtkDataSetCollection.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMInputProperty.h"
#include "vtkSMImageDataToTextureProxy.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "vtkX3DSource.h"
#include "vtkSMOutputPort.h"
#include "vtkSMImageDataToTextureProxy.h"
#include "vtkSMNetworkImageDataSourceProxy.h"

#define TEXTURESGROUP "textures"

vtkStandardNewMacro(vtkSMX3DImporterProxy);
vtkCxxRevisionMacro(vtkSMX3DImporterProxy, "$Revision: 1.2 $");
vtkCxxSetObjectMacro(vtkSMX3DImporterProxy,ActiveTexture,vtkSMImageDataToTextureProxy);
//vtkCxxSetObjectMacro(vtkSMX3DImporterProxy,Copier,vtkSMNetworkImageDataSourceProxy);

//----------------------------------------------------------------------------
vtkSMX3DImporterProxy::vtkSMX3DImporterProxy()
{
  this->ActiveTexture = NULL;
  //this->Copier = NULL;
  this->TextureStatus = -1;
}

//----------------------------------------------------------------------------
vtkSMX3DImporterProxy::~vtkSMX3DImporterProxy()
{
  this->UnRegisterTexture();
  this->SetActiveTexture(NULL);
  //this->SetCopier(NULL);
}

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }
  this->Superclass::CreateVTKObjects();

}

//----------------------------------------------------------------------------
vtkSMImageDataToTextureProxy* vtkSMX3DImporterProxy::CreateOrFindTextureProxy()
{
  vtkSMImageDataToTextureProxy* result = NULL;
  
  for (unsigned int i = 0; i < this->GetNumberOfConsumers(); i++)
    {
    //vtkWarningMacro ( << this->GetConsumerProxy(i)->GetXMLName() );
    result = vtkSMImageDataToTextureProxy::SafeDownCast(this->GetConsumerProxy(i));
    if (result)
      break;
    }
  if (!result)
    {
    result = vtkSMImageDataToTextureProxy::SafeDownCast(
      vtkSMProxyManager::GetProxyManager()->NewProxy("textures", "ImageDataTexture"));
    result->SetConnectionID(this->GetConnectionID());
    result->SetServers(vtkProcessModule::CLIENT|vtkProcessModule::RENDER_SERVER);
    result->UpdateVTKObjects();
    vtkSMInputProperty* input = vtkSMInputProperty::SafeDownCast(result->GetProperty("Input"));
    input->AddInputConnection(this, 1);
    }
  
  //this->SetCopier(vtkSMNetworkImageDataSourceProxy::SafeDownCast(pxm->NewProxy("sources", "NetworkImageDataSource")));
  //vtkSMInputProperty* input = vtkSMInputProperty::SafeDownCast(this->Copier->GetProperty("Input"));
  //input->AddInputConnection(this, 1);
  return result;
}

vtkPVXMLElement* vtkSMX3DImporterProxy::SaveState (vtkPVXMLElement *root)
{
  return this->Superclass::SaveState(root);
}

int	vtkSMX3DImporterProxy::LoadState (vtkPVXMLElement *element, vtkSMProxyLocator *locator)
  {
  return this->Superclass::LoadState(element, locator);
  }

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::UpdateVTKObjects()
{
  this->Superclass::UpdateVTKObjects();
}

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::PostUpdateData()
{
 //vtkWarningMacro(<< "PostUpdateData")
 vtkSMProxyManager* pxm = vtkSMProxyManager::GetProxyManager();
 int textureStatus = GetTextureStatus();
 if (textureStatus != this->TextureStatus || textureStatus == 2)
   {
   if (!this->ActiveTexture)
       this->SetActiveTexture(this->CreateOrFindTextureProxy());

   switch (textureStatus)
      {
      case 0: // Shape has no texture
        UnRegisterTexture();
        break;
      case 1: // Shape has a texture now
        this->ActiveTexture->UpdateImage();
        RegisterTexture();
        break;
      case 2: // Shape's texture has changed
        this->ActiveTexture->UpdateImage();
        break;
     }
    }
 this->TextureStatus = textureStatus;
 
 this->Superclass::PostUpdateData();
}


int vtkSMX3DImporterProxy::GetTextureStatus()
  {
  vtkClientServerStream stream;
  stream  << vtkClientServerStream::Invoke
  << this->GetID()
  << "GetTextureStatus"
  << vtkClientServerStream::End;
  vtkProcessModule::GetProcessModule()->SendStream(this->ConnectionID, 
  this->GetServers(), stream);

  int reply;
  int retVal = vtkProcessModule::GetProcessModule()->GetLastResult(this->ConnectionID,
  vtkProcessModule::GetRootId(this->Servers)).GetArgument(0, 0, &reply);
  
  return retVal ? reply : 0;
  }


//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::RegisterTexture()
{
  vtkSMProxyProperty* textureProperty = vtkSMProxyProperty::SafeDownCast(this->GetProperty("Texture"));
  if (textureProperty)
    {
    textureProperty->SetProxy(0, this->ActiveTexture);
    }
  vtkSMProxyManager::GetProxyManager()->RegisterProxy(TEXTURESGROUP, "X3D Image", this->ActiveTexture);
}

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::UnRegisterTexture()
{
  vtkSMProxyProperty* textureProperty = vtkSMProxyProperty::SafeDownCast(this->GetProperty("Texture"));
  if (textureProperty)
    {
    textureProperty->RemoveAllProxies();
    }
  if (vtkSMProxyManager::GetProxyManager()->IsProxyInGroup(this->ActiveTexture, TEXTURESGROUP))
    {
    vtkSMProxyManager::GetProxyManager()->UnRegisterProxy(this->ActiveTexture);
    }
}

//----------------------------------------------------------------------------
unsigned int vtkSMX3DImporterProxy::GetNumberOfAlgorithmOutputPorts()
{
  return 1;
}

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


