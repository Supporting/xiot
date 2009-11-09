/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkSMNetworkImageDataSourceProxy.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMNetworkImageDataSourceProxy - proxy for an image reader that can read
// an image file from any process and make the data available on all processes.
// .SECTION Description
// vtkSMNetworkImageDataSourceProxy is a proxy for an image reader that can read
// an image file from any process and make the data available on all rendering 
// processes i.e. render server and client.
// It is used to load small images such as those used as textures.

#ifndef __vtkSMNetworkImageDataSourceProxy_h
#define __vtkSMNetworkImageDataSourceProxy_h

#include "vtkSMSourceProxy.h"
#include "vtkProcessModule.h" //for flags.
#include "vtkWeakPointer.h"

class vtkImageData;

class VTK_EXPORT vtkSMNetworkImageDataSourceProxy : public vtkSMSourceProxy
{
public:
  static vtkSMNetworkImageDataSourceProxy* New();
  vtkTypeRevisionMacro(vtkSMNetworkImageDataSourceProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);


  //BTX
  enum {
    CLIENT = vtkProcessModule::CLIENT,
    DATA_SERVER_ROOT = vtkProcessModule::DATA_SERVER_ROOT,
    RENDER_SERVER_ROOT = vtkProcessModule::RENDER_SERVER_ROOT
  };
  //ETX
  
  // Description:
  // Set the process on which the image is available.
  void SetSourceProcess(int);
  void SetSourceProcessToClient()
    { this->SetSourceProcess(CLIENT); }
  void SetSourceProcessToDataServerRoot()
    { this->SetSourceProcess(DATA_SERVER_ROOT); }
  void SetSourceProcessToRenderServerRoot()
    { this->SetSourceProcess(RENDER_SERVER_ROOT); }

  virtual void UpdateVTKObjects()
    { this->Superclass::UpdateVTKObjects(); }

  
 virtual void AddInput(unsigned int inputPort,
                        vtkSMSourceProxy* input,
                        unsigned int outputPort,
                        const char* method);

 void UpdateImage();

//BTX
protected:
  vtkSMNetworkImageDataSourceProxy();
  ~vtkSMNetworkImageDataSourceProxy();

  int SourceProcess;

  vtkWeakPointer<vtkSMProxy> InputProxy;
  int OutputPort;

private:
  vtkSMNetworkImageDataSourceProxy(const vtkSMNetworkImageDataSourceProxy&); // Not implemented
  void operator=(const vtkSMNetworkImageDataSourceProxy&); // Not implemented
//ETX
};

#endif

