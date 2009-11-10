/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkX3DSource.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkX3DSource - Extracts one geometry from a collection imported 
// with vtkX3DImporter
// .SECTION Description
// Extracts one geometry - in terms of X3D this is one Shape node - and makes
// it's geometry available as vtkMultiBlockDataSet at port 0 and it's texture
// as vtkImageData at port 1

#ifndef __vtkX3DSource_h
#define __vtkX3DSource_h

#include "vtkMultiBlockDataSetAlgorithm.h"

class vtkMultiBlockDataSet;
class vtkX3DImporter;
class vtkStringArray;
class vtkDataSetCollection;
class vtkImageData;
class vtkActorCollection;
class vtkActor; 

class VTK_EXPORT vtkX3DSource : public vtkMultiBlockDataSetAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkX3DSource,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkX3DSource *New();

  // Description:
  // X3D file name.  Set
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description: 
  // Descide whether to generate color arrays from material description
  // or not.
  vtkSetMacro(Color,int);
  vtkGetMacro(Color,int);
  vtkBooleanMacro(Color,int);

  // Description: 
  // Calculates the number of Shape nodes in the X3D file. 
  // This independs on the current FileName only, no update
  // is performed
  virtual int GetNumberOfShapes();

  // Description: 
  // Set the n'th shape node to extract the geometry/texture from
  vtkGetMacro(ShapeNumber,int);
  vtkSetMacro(ShapeNumber,int);
  vtkGetVector2Macro(ShapeRange, int);

  // Description: 
  // Get the status at port 1 for the texture. Returns:
  // -1 - uninitalized
  //  0 - not texture available
  //  1 - texture available
  //  2 - texture data has changed
  vtkGetMacro(TextureStatus,int);

  static int CanReadFile(const char *filename);

protected:
  vtkX3DSource();
  ~vtkX3DSource();

  int RequestData(vtkInformation*, 
                  vtkInformationVector**, 
                  vtkInformationVector*);

  void InitializeImporter();
  void CopyImporterToOutputs(vtkMultiBlockDataSet*, vtkImageData* texture);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);

  char* FileName;
  vtkX3DImporter *Importer;
  int Color;

  // Shape related properties
  int NumberOfShapes;
  int ShapeRange[2];
  int ShapeNumber;

  // Texture related properties
  int TextureStatus;


private:
  vtkX3DSource(const vtkX3DSource&);  // Not implemented.
  void operator=(const vtkX3DSource&);  // Not implemented.
};

#endif

