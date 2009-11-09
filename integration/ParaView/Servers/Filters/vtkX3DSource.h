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
// .NAME vtkX3DSource - Converts importer to a source.
// .SECTION Description
// Since paraview can only use vtkSources, I am wrapping the X3D importer
// as a source.  I will loose lights, texture maps and colors,

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
  // Descided whether to generate color arrays or not.
  vtkSetMacro(Color,int);
  vtkGetMacro(Color,int);
  vtkBooleanMacro(Color,int);

  vtkGetObjectMacro(Texture, vtkImageData);
  //virtual void SetTexture(vtkImageData *);

  virtual int GetNumberOfShapes();
  vtkGetVector2Macro(ShapeRange, int);

  vtkGetMacro(ShapeNumber,int);
  vtkSetMacro(ShapeNumber,int);

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
  vtkImageData* Texture;
  int TextureStatus;


private:
  vtkX3DSource(const vtkX3DSource&);  // Not implemented.
  void operator=(const vtkX3DSource&);  // Not implemented.
};

#endif

