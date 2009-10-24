/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkX3DSource.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkX3DSource.h"

#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkAppendPolyData.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkUnsignedCharArray.h"
#include "vtkFloatArray.h"
#include "vtkX3DImporter.h"
#include "vtkImageData.h"
#include "vtkStringArray.h"
#include "vtkDataSetCollection.h"
#include "vtkSmartPointer.h"


vtkCxxRevisionMacro(vtkX3DSource, "$Revision: 1.12 $");
vtkStandardNewMacro(vtkX3DSource);

//------------------------------------------------------------------------------
vtkX3DSource::vtkX3DSource()
{
  this->FileName = NULL;
  this->Importer = NULL;
  this->ImageCollection = vtkDataSetCollection::New();
  this->Color = 1;
  this->Append = 0;
  this->TextureInfo = vtkStringArray::New();

  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//------------------------------------------------------------------------------
vtkX3DSource::~vtkX3DSource()
{
  this->SetFileName(NULL);
  if (this->Importer)
    {
    this->Importer->Delete();
    this->Importer = NULL;
    }
  if (this->TextureInfo)
    {
    this->TextureInfo->Delete();
}
  if(this->ImageCollection)
    {
    this->ImageCollection->Delete();
    }
}

//------------------------------------------------------------------------------
vtkImageData* vtkX3DSource::GetActiveImage()
{
return HasTextures() ? vtkImageData::SafeDownCast(GetImageCollection()->GetItem(0)) : NULL;
}

//------------------------------------------------------------------------------
int vtkX3DSource::HasTextures()
{
return GetImageCollection()->GetNumberOfItems();;
}
  
//-----------------------------------------------------------------------------
int vtkX3DSource::CanReadFile(const char *filename)
{
  FILE *fd = fopen(filename, "r");
  if (!fd) return 0;

  // TODO: File detection
  return 1;
}

//------------------------------------------------------------------------------
int vtkX3DSource::RequestData(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  vtkInformation* info = outputVector->GetInformationObject(0);

  vtkDataObject* doOutput = info->Get(vtkDataObject::DATA_OBJECT());
  vtkMultiBlockDataSet* output = vtkMultiBlockDataSet::SafeDownCast(doOutput);
  if (!output)
    {
    return 0;
    }

  if (this->Importer == NULL)
    {
    this->InitializeImporter();
    }
  this->CopyImporterToOutputs(output);

  return 1;
}

//------------------------------------------------------------------------------
void vtkX3DSource::InitializeImporter()
{
  if (this->Importer)
    {
    this->Importer->Delete();
    this->Importer = NULL;
    }
  this->Importer = vtkX3DImporter::New();
  this->Importer->SetFileName(this->FileName);
  this->Importer->Read();
}

//------------------------------------------------------------------------------
void vtkX3DSource::CopyImporterToOutputs(vtkMultiBlockDataSet* mbOutput)
{
  vtkRenderer* ren;
  vtkActorCollection* actors;
  vtkActor* actor;
  vtkPolyDataMapper* mapper;
  vtkPolyData* input;
  vtkPolyData* output;
  int idx;
  int numPoints, numCells;
  int arrayCount = 0;
  int ptIdx;
  vtkAppendPolyData* append = NULL;

  if (this->Importer == NULL)
    {
    return;
    }

  if (this->Append)
    {
    append = vtkAppendPolyData::New();
    }

  ren = this->Importer->GetRenderer();
  actors = ren->GetActors();
  actors->InitTraversal();
  idx = 0;
  while ( (actor = actors->GetNextActor()) )
    {
    if (actor->GetTexture())
      {
      this->TextureInfo->InsertNextValue("Texture");
      vtkSmartPointer<vtkImageData> id = vtkSmartPointer<vtkImageData>::New();
      id->ShallowCopy(actor->GetTexture()->GetInput());
      this->ImageCollection->AddItem(id);
      }
    mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
    if (mapper)
      {
      input = mapper->GetInput();
      input->Update();

      output = vtkPolyData::New();

      if (!append)
        {
        mbOutput->SetBlock(idx, output);
        }

      vtkTransformPolyDataFilter *tf = vtkTransformPolyDataFilter::New();
      vtkTransform *trans = vtkTransform::New();
      tf->SetInput(input);
      tf->SetTransform(trans);
      trans->SetMatrix(actor->GetMatrix());
      input = tf->GetOutput();
      input->Update();

      output->CopyStructure(input);
      // Only copy well formed arrays.
      numPoints = input->GetNumberOfPoints();
      vtkPointData* ipd = input->GetPointData();	
      vtkPointData* opd = output->GetPointData();	

      // Set vertex normals if they exist and have the expected size
      if(ipd->GetNormals())
        {
        if(ipd->GetNormals()->GetNumberOfTuples() == numPoints)
          {
          opd->SetNormals(ipd->GetNormals());
          opd->GetNormals()->SetName("X3D Vertex Normals");
          }
        else if (ipd->GetNormals()->GetNumberOfTuples() >  numPoints)
          {
          vtkFloatArray *normalArray = vtkFloatArray::New();
          normalArray->SetNumberOfComponents(3);
          normalArray->DeepCopy(ipd->GetNormals());
          normalArray->SetName("X3D Vertex Normals");
          normalArray->Resize(numPoints);
          opd->SetNormals(normalArray);
          normalArray->Delete();
          }
        else	
          vtkDebugMacro(<< "Vertex Normals have wrong size");
        }

      // Set vertex colors if they exist and have the expected size
      if(ipd->GetScalars())
        {
        if(ipd->GetScalars()->GetNumberOfTuples() == numPoints)
          {
          opd->SetScalars(ipd->GetScalars());
          opd->GetScalars()->SetName("X3D Vertex Colors");
          } // This could happen when sharing point data, so we create a own copy here
        else if (ipd->GetScalars()->GetNumberOfTuples() >  numPoints)
          {
          vtkUnsignedCharArray *colorArray = vtkUnsignedCharArray::New();
          colorArray->DeepCopy(ipd->GetScalars());
          colorArray->SetName("X3D Vertex Colors");
          colorArray->Resize(numPoints);
          opd->SetScalars(colorArray);
          colorArray->Delete();
          }
        else
          vtkDebugMacro(<< "Vertex Colors have wrong size");
        }

      // Set vertex colors if they exist and have the expected size
      if(ipd->GetTCoords())
        if(ipd->GetTCoords()->GetNumberOfTuples() == numPoints)
          {
          opd->SetTCoords(ipd->GetTCoords());
          opd->GetTCoords()->SetName("X3D TCoords");
          }
        else if (ipd->GetTCoords()->GetNumberOfTuples() > numPoints)
          {
          vtkFloatArray *tcoordsArray = vtkFloatArray::New();
          tcoordsArray->DeepCopy(ipd->GetTCoords());
          tcoordsArray->SetName("X3D TCoords");
          tcoordsArray->Resize(numPoints);
          opd->SetTCoords(tcoordsArray);
          tcoordsArray->Delete();
          }
        else
          vtkDebugMacro(<< "TextureCoordinates have wrong size");

        // Process Cell Data 
        numCells = input->GetNumberOfCells();
        vtkCellData* icd = input->GetCellData();	
        vtkCellData* ocd = output->GetCellData();	

        // Cell normals
        if(icd->GetNormals())
          {
          if(icd->GetNormals()->GetNumberOfTuples() == numCells)
            {
            ocd->SetNormals(icd->GetNormals());
            ocd->GetNormals()->SetName("X3D Cell Normals");
            }
          else if (icd->GetNormals()->GetNumberOfTuples() >  numCells)
            {
            vtkFloatArray *normalArray = vtkFloatArray::New();
            normalArray->SetNumberOfComponents(3);
            normalArray->DeepCopy(icd->GetNormals());
            normalArray->SetName("X3D Cell Normals");
            normalArray->Resize(numCells);
            ocd->SetNormals(normalArray);
            normalArray->Delete();
            }
          else	
            vtkDebugMacro(<< "Cell normals have wrong size");
          }

        // Process Cell Colors
        if(icd->GetScalars())
          {
          if(icd->GetScalars()->GetNumberOfTuples() == numCells)
            {
            ocd->SetScalars(icd->GetScalars());
            ocd->GetScalars()->SetName("X3D Cell Colors");
            }
          else if (icd->GetScalars()->GetNumberOfTuples() >  numCells)
            {
            vtkUnsignedCharArray *colorArray = vtkUnsignedCharArray::New();
            colorArray->DeepCopy(icd->GetScalars());
            colorArray->SetName("X3D Cell Normals");
            colorArray->Resize(numCells);
            ocd->SetScalars(colorArray);
            colorArray->Delete();
            }
          else	
            vtkDebugMacro(<< "Cell colors have wrong size");
          }

        if (this->Color)
          {
          vtkUnsignedCharArray *colorArray = vtkUnsignedCharArray::New();
          unsigned char r, g, b;
          double* actorColor;

          actorColor = actor->GetProperty()->GetColor();
          r = static_cast<unsigned char>(actorColor[0]*255.0);
          g = static_cast<unsigned char>(actorColor[1]*255.0);
          b = static_cast<unsigned char>(actorColor[2]*255.0);
          colorArray->SetName("X3D Material Color");
          colorArray->SetNumberOfComponents(3);
          for (ptIdx = 0; ptIdx < numPoints; ++ptIdx)
            {
            colorArray->InsertNextValue(r);
            colorArray->InsertNextValue(g);
            colorArray->InsertNextValue(b);
            }
          output->GetPointData()->AddArray(colorArray);
          colorArray->Delete();
          colorArray = NULL;
          }
        if (append)
          {
          append->AddInput(output);
          }
        output->Delete();
        output = NULL;

        ++idx;
        tf->Delete();
        tf = NULL;
        trans->Delete();
        trans = NULL;
      }
    
    }

  if (append)
    {
    append->Update();
    vtkPolyData* newOutput = vtkPolyData::New();
    newOutput->ShallowCopy(append->GetOutput());
    mbOutput->SetBlock(0, newOutput);
    newOutput->Delete();
    append->Delete();
    }
}



//------------------------------------------------------------------------------
void vtkX3DSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if (this->FileName)
    {
    os << indent << "FileName: " << this->FileName << endl;
    }
  os << indent << "Color: " << this->Color << endl;
  os << indent << "Append: " << this->Append << endl;
}

