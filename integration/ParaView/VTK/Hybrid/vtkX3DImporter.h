/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVRMLImporter.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkX3DImporter - imports X3D files.
// .SECTION Description
//
// vtkX3DImporter imports X3D files into vtk.
// .SECTION Caveats
//
// These nodes are currently supported:
//   Appearance 
//   Background (first, background color only)
//   Box
//   Color
//   Cone
//   Coordinate
//   Cylinder
//   DirectionalLight
//   IndexedFaceSet
//   IndexedLineSet
//   ImageTexture (relative filename only)
//   Material
//   NavigationInfo (for headlight)
//   Normal
//   PixelTexture
//   PointSet
//   Shape
//   Sphere
//   TextureCoordinate
//   Transform
//   Viewpoint (first)
//
// As you can see this implementation focuses on getting the geometry
// translated.  The routes and scripting nodes are ignored since they deal
// with directly accessing a nodes internal structure based on the X3D
// spec. 
// .SECTION Thanks
// X3DExporter is contributed by Christophe Mouton at EDF and implemented 
// by Kristian Sons at Supporting (xiot@supporting.com).
//
// .SECTION See Also
// vtkImporter
#ifndef __vtkX3DImporter_h
#define __vtkX3DImporter_h

#include "vtkImporter.h"


class VTK_HYBRID_EXPORT vtkX3DImporter : public vtkImporter
{
public:
  static vtkX3DImporter *New();
  vtkTypeRevisionMacro(vtkX3DImporter,vtkImporter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the name of the file to read.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  vtkSetMacro(CalculateNormals  ,int);
  vtkGetMacro(CalculateNormals ,int);
  vtkBooleanMacro(CalculateNormals ,int);

  virtual int ImportBegin ();
  virtual void ImportEnd ();

protected:
  vtkX3DImporter();
  ~vtkX3DImporter();
 
  char *FileName;
  int CalculateNormals;

private:

  vtkX3DImporter(const vtkX3DImporter&); // Not implemented.
  void operator=(const vtkX3DImporter&); // Not implemented.
};


#endif
