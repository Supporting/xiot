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
  this->Color = 1;

  this->TextureStatus = -1;

  this->NumberOfShapes = 0;
  this->ShapeNumber = 0;
  this->ShapeRange[0] = 0;
  this->ShapeRange[1] = -1;

  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(2);
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
}


//------------------------------------------------------------------------------
int vtkX3DSource::GetNumberOfShapes()
{
  if (!this->FileName || !CanReadFile(this->FileName))
    {
    return -1;
    }
  int shapeCount = this->Importer->GetNumberOfShapes(this->FileName);
  this->ShapeRange[1] = shapeCount-1;
  return shapeCount;
}


//-----------------------------------------------------------------------------
int vtkX3DSource::CanReadFile(const char *filename)
{
  FILE *fd = fopen(filename, "r");
  if (!fd) return 0;

  // TODO: File detection

  fclose(fd);
  return 1;
}

//------------------------------------------------------------------------------
int vtkX3DSource::RequestData(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  if (!this->FileName)
    return 1;

  vtkInformation* info0 = outputVector->GetInformationObject(0);
  vtkInformation* info1 = outputVector->GetInformationObject(1);

  vtkMultiBlockDataSet* geometry = vtkMultiBlockDataSet::SafeDownCast(info0->Get(vtkDataObject::DATA_OBJECT()));
  vtkImageData* texture = vtkImageData::SafeDownCast(info1->Get(vtkDataObject::DATA_OBJECT()));
  if (!geometry || !texture)
    {
    return 0;
    }

  if (this->Importer == NULL)
    {
    this->InitializeImporter();
    }
  this->CopyImporterToOutputs(geometry, texture);

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
void vtkX3DSource::CopyImporterToOutputs(vtkMultiBlockDataSet* mbOutput, vtkImageData* texture)
  {
  vtkRenderer* ren;
  vtkActorCollection* actors;
  vtkActor* actor;
  vtkPolyDataMapper* mapper;
  vtkPolyData* input;
  vtkPolyData* output;
  int numPoints, numCells;
  int arrayCount = 0;
  int ptIdx;
  vtkMultiBlockDataSet* comp = NULL;

  if (this->Importer == NULL)
    {
    return;
    }

  ren = this->Importer->GetRenderer();
  actors = ren->GetActors();

  if (actors->GetNumberOfItems() <= this->ShapeNumber)
    {
    return;
    }

  actor = static_cast<vtkActor*>(actors->GetItemAsObject(this->ShapeNumber));

  if (actor->GetTexture())
    {
    texture->ShallowCopy(actor->GetTexture()->GetInput());
    this->TextureStatus = this->TextureStatus == -1 ? 1 : 2;
    }
  else
    this->TextureStatus = 0;

  mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
  if (mapper)
    {
    input = mapper->GetInput();
    input->Update();

    output = vtkPolyData::New();
    mbOutput->SetBlock(0, output);

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
      
      output->Delete();
      output = NULL;

      tf->Delete();
      tf = NULL;
      trans->Delete();
      trans = NULL;
    }
  }


//----------------------------------------------------------------------------
int vtkX3DSource::FillOutputPortInformation(
  int port, vtkInformation* info)
{
  if (port==1)
    {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
    return 1;
    }
  else return this->Superclass::FillOutputPortInformation(port, info);
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
}


