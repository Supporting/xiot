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
// .NAME vtkSMX3DImporterProxy - Proxy for vtkSource.
// .SECTION Description
// Pipes port 0 of the vtkSource to the standard pipeline whereas from the
// image data at port 1, a ImageDataTexture is created, registered in the
// texture manager and is also available as proxy property 'Texture'
#ifndef __vtkSMX3DImporterProxy_h
#define __vtkSMX3DImporterProxy_h

#include "vtkSMSourceProxy.h"

class vtkImageData;
class vtkSMImageDataToTextureProxy;

class VTK_EXPORT vtkSMX3DImporterProxy : public vtkSMSourceProxy
{
public:
  static vtkSMX3DImporterProxy* New();
  vtkTypeRevisionMacro(vtkSMX3DImporterProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates the output port proxies for this filter. 
  // Only the first algorithm output is exposed
 virtual unsigned int GetNumberOfAlgorithmOutputPorts();

//BTX
protected:
  vtkSMX3DImporterProxy();
  ~vtkSMX3DImporterProxy();

  virtual void PostUpdateData();

  virtual void RegisterTexture();
  virtual void UnRegisterTexture();

  virtual void SetActiveTexture(vtkSMImageDataToTextureProxy *);
  vtkSMImageDataToTextureProxy *CreateOrFindTextureProxy();
  
  int GetTextureStatus();
  int TextureStatus;
  vtkSMImageDataToTextureProxy* ActiveTexture;

private:
  vtkSMX3DImporterProxy(const vtkSMX3DImporterProxy&); // Not implemented
  void operator=(const vtkSMX3DImporterProxy&); // Not implemented
//ETX
};

#endif

