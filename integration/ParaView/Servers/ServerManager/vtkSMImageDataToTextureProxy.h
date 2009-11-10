/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkSMImageDataToTextureProxy.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMImageDataToTextureProxy - proxy for a vtkTexture.
// .SECTION Description
// Combines vtkTexture and vtkNetworkImageSource. This makes it simple for
// applications to create textures from image files where the image file is
// present on only one process, say client, but is needed for rendering on the
// render server and the data server.
#ifndef __vtkSMImageDataToTextureProxy_h
#define __vtkSMImageDataToTextureProxy_h

#include "vtkSMSourceProxy.h"

class vtkImageData;
class vtkSMNetworkImageDataSourceProxy;

class VTK_EXPORT vtkSMImageDataToTextureProxy : public vtkSMSourceProxy
{
public:
  static vtkSMImageDataToTextureProxy* New();
  vtkTypeRevisionMacro(vtkSMImageDataToTextureProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Returns the client side image if one has been loaded successfully.
  vtkImageData* GetLoadedImage();
  
  // Description:
  // Updates the images and distributes the data to the texture processes.
  void UpdateImage();

 virtual void AddInput(unsigned int inputPort,
                        vtkSMSourceProxy* input,
                        unsigned int outputPort,
                        const char* method);

//BTX
protected:
  vtkSMImageDataToTextureProxy();
  ~vtkSMImageDataToTextureProxy();

  virtual void CreateVTKObjects();
  
  virtual void SetCopier(vtkSMNetworkImageDataSourceProxy *);
  vtkSMNetworkImageDataSourceProxy* Copier;

private:
  vtkSMImageDataToTextureProxy(const vtkSMImageDataToTextureProxy&); // Not implemented
  void operator=(const vtkSMImageDataToTextureProxy&); // Not implemented
//ETX
};

#endif

