/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkSMX3DImporterProxy.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMX3DImporterProxy - proxy for a vtkTexture.
// .SECTION Description
// Combines vtkTexture and vtkNetworkImageSource. This makes it simple for
// applications to create textures from image files where the image file is
// present on only one process, say client, but is needed for rendering on the
// render server and the data server.
#ifndef __vtkSMX3DImporterProxy_h
#define __vtkSMX3DImporterProxy_h

#include "vtkSMSourceProxy.h"

class vtkImageData;
class vtkSMImageDataToTextureProxy;
class vtkSMNetworkImageDataSourceProxy;

class VTK_EXPORT vtkSMX3DImporterProxy : public vtkSMSourceProxy
{
public:
  static vtkSMX3DImporterProxy* New();
  vtkTypeRevisionMacro(vtkSMX3DImporterProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates the output port proxiess for this filter. 
  // Only the first algorithm output is exposed
 virtual unsigned int GetNumberOfAlgorithmOutputPorts();

  virtual vtkPVXMLElement *SaveState (vtkPVXMLElement *root);
  virtual int 	LoadState (vtkPVXMLElement *element, vtkSMProxyLocator *locator);

//BTX
protected:
  vtkSMX3DImporterProxy();
  ~vtkSMX3DImporterProxy();

  virtual void CreateVTKObjects();
  virtual void UpdateVTKObjects();
  virtual void PostUpdateData();

  virtual void UnRegisterTexture();
  virtual void RegisterTexture();

  virtual void SetActiveTexture(vtkSMImageDataToTextureProxy *);
  //virtual void SetCopier(vtkSMNetworkImageDataSourceProxy *);
  vtkSMImageDataToTextureProxy *CreateOrFindTextureProxy();
  int GetTextureStatus();

  int TextureStatus;
  vtkSMImageDataToTextureProxy* ActiveTexture;
  //vtkSMNetworkImageDataSourceProxy* Copier;

private:
  vtkSMX3DImporterProxy(const vtkSMX3DImporterProxy&); // Not implemented
  void operator=(const vtkSMX3DImporterProxy&); // Not implemented
//ETX
};

#endif

