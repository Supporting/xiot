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

#define TEXTURESGROUP "textures"

vtkStandardNewMacro(vtkSMX3DImporterProxy);
vtkCxxRevisionMacro(vtkSMX3DImporterProxy, "$Revision: 1.2 $");
//----------------------------------------------------------------------------
vtkSMX3DImporterProxy::vtkSMX3DImporterProxy()
{
  this->NeedsTextureUpdate = 0; // Not until VTK object is updated
  this->ActiveTexture = NULL;
}

//----------------------------------------------------------------------------
vtkSMX3DImporterProxy::~vtkSMX3DImporterProxy()
{
 vtkSMProxyManager* pxm = vtkSMProxyManager::GetProxyManager();
 pxm->UnRegisterProxy(TEXTURESGROUP, "X3D Image");
}

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }
  this->Superclass::CreateVTKObjects();
  
  vtkSMProxyManager* pxm = vtkSMProxyManager::GetProxyManager();
  this->ActiveTexture = pxm->NewProxy("textures", "ImageDataTexture");
  this->ActiveTexture->SetConnectionID(this->GetConnectionID());


}

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::UpdateVTKObjects()
{
  this->Superclass::UpdateVTKObjects();
}

//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::PostUpdateData()
{
 vtkSMProxyManager* pxm = vtkSMProxyManager::GetProxyManager();

 vtkDataSetCollection* collection = vtkDataSetCollection::New();
 pxm->GetProxies(TEXTURESGROUP, "X3D Image", collection);
 if (!collection->GetNumberOfItems() && this->HasTextures())
   {
   pxm->RegisterProxy(TEXTURESGROUP, "X3D Image", this->ActiveTexture);
   // Set up the texture proxy
   vtkSMInputProperty* input = vtkSMInputProperty::SafeDownCast(
     this->ActiveTexture->GetProperty("Input"));
   input->AddProxy(this,1);

   vtkSMIntVectorProperty* sp = vtkSMIntVectorProperty::SafeDownCast(
     this->ActiveTexture->GetProperty("SourceProcess"));
   sp->SetElement(0, vtkProcessModule::DATA_SERVER); // Data Server
 
   this->ActiveTexture->UpdateVTKObjects();
   }
 collection->Delete();
 this->Superclass::PostUpdateData();
}

int vtkSMX3DImporterProxy::HasTextures()
  {
  vtkClientServerStream stream;
  stream  << vtkClientServerStream::Invoke
  << this->GetID()
  << "HasTextures"
  << vtkClientServerStream::End;
  vtkProcessModule::GetProcessModule()->SendStream(this->ConnectionID, 
  this->GetServers(), stream);

  int reply;
  int retVal = vtkProcessModule::GetProcessModule()->GetLastResult(this->ConnectionID,
  vtkProcessModule::GetRootId(this->Servers)).GetArgument(0, 0, &reply);
  
  return retVal && reply;
  }



//----------------------------------------------------------------------------
void vtkSMX3DImporterProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


