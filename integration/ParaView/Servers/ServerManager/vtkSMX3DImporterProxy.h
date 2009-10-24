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

class VTK_EXPORT vtkSMX3DImporterProxy : public vtkSMSourceProxy
{
public:
  static vtkSMX3DImporterProxy* New();
  vtkTypeRevisionMacro(vtkSMX3DImporterProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

//BTX
protected:
  vtkSMX3DImporterProxy();
  ~vtkSMX3DImporterProxy();

  virtual void CreateVTKObjects();
  virtual void UpdateVTKObjects();
  
  virtual void PostUpdateData();

  int HasTextures();

  int NeedsTextureUpdate;

  vtkSMProxy* ActiveTexture;

private:
  vtkSMX3DImporterProxy(const vtkSMX3DImporterProxy&); // Not implemented
  void operator=(const vtkSMX3DImporterProxy&); // Not implemented
//ETX
};

#endif

