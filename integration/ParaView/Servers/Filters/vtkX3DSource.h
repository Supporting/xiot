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

  // Description:
  // This method allows all parts to be put into a single output.
  // By default this flag is on.
  vtkSetMacro(Append,int);
  vtkGetMacro(Append,int);
  vtkBooleanMacro(Append,int);

  vtkGetObjectMacro(TextureInfo, vtkStringArray);
  vtkGetObjectMacro(ImageCollection, vtkDataSetCollection);

  vtkImageData* GetActiveImage();
  int HasTextures();
  
  static int CanReadFile(const char *filename);

protected:
  vtkX3DSource();
  ~vtkX3DSource();

  int RequestData(vtkInformation*, 
                  vtkInformationVector**, 
                  vtkInformationVector*);

  void InitializeImporter();
  void CopyImporterToOutputs(vtkMultiBlockDataSet*);

  char* FileName;
  vtkX3DImporter *Importer;
  int Color;
  int Append;
  vtkStringArray* TextureInfo;
  vtkDataSetCollection* ImageCollection;


private:
  vtkX3DSource(const vtkX3DSource&);  // Not implemented.
  void operator=(const vtkX3DSource&);  // Not implemented.
};

#endif

