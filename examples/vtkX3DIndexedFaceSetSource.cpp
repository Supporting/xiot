#include "vtkX3DIndexedFaceSetSource.h"

#include <cassert>
#include <map>

#include "vtkObjectFactory.h"
#include "vtkIdTypeArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyDataNormals.h"
#include "vtkMath.h"
#include "vtkHedgeHog.h" //TEMP
#include "vtkAppendPolyData.h" //TEMP


vtkCxxRevisionMacro(vtkX3DIndexedFaceSetSource, "$Revision: 1.19 $");
vtkStandardNewMacro(vtkX3DIndexedFaceSetSource);

struct Vertex {
	int tc, normal, color;

	bool operator<(const Vertex &v) const {
		if (normal < v.normal) return true;
		if (normal > v.normal) return false;
		if (tc < v.tc) return true;
		if (tc > v.tc) return false;
		return color < v.color;
	}

  bool operator==(const Vertex &v) const {
    return (normal == v.normal) && (color == v.color) && (tc == v.tc);
	}
};

typedef std::map<int, Vertex> VertexMap;

vtkX3DIndexedFaceSetSource::vtkX3DIndexedFaceSetSource()
{
	this->ColorPerVertex = 1;
	this->NormalPerVertex = 1;

  this->CreaseAngle = 0;

	this->CoordIndex = NULL;
	this->NormalIndex = NULL;
	this->TexCoordIndex = NULL;
	this->ColorIndex = NULL;

	this->Coords = NULL;
	this->Normals = NULL;
	this->TexCoords = NULL;
	this->Colors = NULL;

	this->SetNumberOfInputPorts(0);
}

vtkX3DIndexedFaceSetSource::~vtkX3DIndexedFaceSetSource()
{
	if (this->CoordIndex)
	{
		this->CoordIndex->Delete();
	}

	if (this->NormalIndex)
	{
		this->NormalIndex->Delete();
	}

	if (this->TexCoordIndex)
	{
		this->TexCoordIndex->Delete();
	}

	if (this->ColorIndex)
	{
		this->ColorIndex->Delete();
	}

	if (this->Coords)
	{
		this->Coords->Delete();
	}

	if (this->Normals)
	{
		this->Normals->Delete();
	}

	if (this->TexCoords)
	{
		this->TexCoords->Delete();
	}

	if (this->Colors)
	{
		this->Colors->Delete();
	}
}

void vtkX3DIndexedFaceSetSource::SetCoordIndex(vtkIdTypeArray * i)
{
	if ( i != this->CoordIndex)
	{
		if (this->CoordIndex)
		{
			this->CoordIndex->UnRegister(this);
		}
		this->CoordIndex = i;
		if (this->CoordIndex)
		{
			this->CoordIndex->Register(this);
		}
		this->Modified();
	}
}

void vtkX3DIndexedFaceSetSource::SetNormalIndex(vtkIdTypeArray * i)
{
	if ( i != this->NormalIndex)
	{
		if (this->NormalIndex)
		{
			this->NormalIndex->UnRegister(this);
		}
		this->NormalIndex = i;
		if (this->NormalIndex)
		{
			this->NormalIndex->Register(this);
		}
		this->Modified();
	}
}

void vtkX3DIndexedFaceSetSource::SetColorIndex(vtkIdTypeArray * i)
{
	if ( i != this->ColorIndex)
	{
		if (this->ColorIndex)
		{
			this->ColorIndex->UnRegister(this);
		}
		this->ColorIndex = i;
		if (this->ColorIndex)
		{
			this->ColorIndex->Register(this);
		}
		this->Modified();
	}
}

void vtkX3DIndexedFaceSetSource::SetTexCoordIndex(vtkIdTypeArray * i)
{
	if ( i != this->TexCoordIndex)
	{
		if (this->TexCoordIndex)
		{
			this->TexCoordIndex->UnRegister(this);
		}
		this->TexCoordIndex = i;
		if (this->TexCoordIndex)
		{
			this->TexCoordIndex->Register(this);
		}
		this->Modified();
	}
}

void vtkX3DIndexedFaceSetSource::SetCoords(vtkPoints * i)
{
	if ( i != this->Coords)
	{
		if (this->Coords)
		{
			this->Coords->UnRegister(this);
		}
		this->Coords = i;
		if (this->Coords)
		{
			this->Coords->Register(this);
		}
		this->Modified();
	}
}

void vtkX3DIndexedFaceSetSource::SetNormals(vtkFloatArray * i)
{
	if (i->GetNumberOfComponents() != 3)
	{
		vtkErrorMacro(<<"Number of components for Normals must be 3");
		return;
	}
	if ( i != this->Normals)
	{
		if (this->Normals)
		{
			this->Normals->UnRegister(this);
		}
		this->Normals = i;
		if (this->Normals)
		{
			this->Normals->Register(this);
		}
		this->Modified();
	}
}

void vtkX3DIndexedFaceSetSource::SetTexCoords(vtkFloatArray * i)
{
	if (i->GetNumberOfComponents() != 2)
	{
		vtkErrorMacro(<<"Number of components for TexCoords must be 2");
		return;
	}
	if ( i != this->TexCoords)
	{
		if (this->TexCoords)
		{
			this->TexCoords->UnRegister(this);
		}
		this->TexCoords = i;
		if (this->TexCoords)
		{
			this->TexCoords->Register(this);
		}
		this->Modified();
	}
}

void vtkX3DIndexedFaceSetSource::SetColors(vtkUnsignedCharArray * i)
{
	if ( i != this->Colors)
	{
		if (this->Colors)
		{
			this->Colors->UnRegister(this);
		}
		this->Colors = i;
		if (this->Colors)
		{
			this->Colors->Register(this);
		}
		this->Modified();
	}
}

// Generate normals for polygon meshes
int vtkX3DIndexedFaceSetSource::RequestData(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inputVector),
	vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!GetCoordIndex() || GetCoordIndex()->GetNumberOfTuples() == 0)
	{
		// E1
		vtkErrorMacro(<<"No coordinate index given.");
		return 1;
	}

	if (!GetCoords() || GetCoords()->GetNumberOfPoints() == 0)
	{
		// E2
		vtkErrorMacro(<<"No coordinates given.");
		return 1;
	}

  // Make shure, that the index ends with an -1. This is important
	// for later processing
	if (this->CoordIndex->GetValue(this->CoordIndex->GetNumberOfTuples() -1) != -1)
		this->CoordIndex->InsertNextValue(-1);

	// Create PolyData object
	vtkPolyData* pd = vtkPolyData::New();

  // ----
  // Start Vertex position and index processing
  // ----
  vtkCellArray* cells = vtkCellArray::New();
  vtkPoints* points = vtkPoints::New();

  int faceSize = 0;
  vtkIdType* index = this->CoordIndex->GetPointer(0);
  vtkIdType* faceStart = index;
  const vtkIdType* end = this->CoordIndex->GetPointer(this->CoordIndex->GetNumberOfTuples());
  while(index != end)
    {
      while(*index != -1)
        {
        index++; faceSize++;
        }
      cells->InsertNextCell(faceSize);
      while(faceStart != index)
        {
        cells->InsertCellPoint(*faceStart);
        faceStart++;
        }
      faceSize = 0;
      index++; faceStart++;
    }

  bool normalPerVertexIndexed = this->Normals && this->NormalPerVertex &&  this->NormalIndex;
  bool colorPerVertexIndexed = this->Colors && this->ColorPerVertex &&  this->ColorIndex;
  bool textureIndexed = this->TexCoords && this->TexCoordIndex;

	if(normalPerVertexIndexed || colorPerVertexIndexed || textureIndexed) 
    {
    int position = 0, vertexNr = 0;
    VertexMap vmap;
    vtkUnsignedCharArray* newColors = NULL;
    vtkFloatArray* newNormals = NULL;
    vtkFloatArray* newTexCoords = NULL;
    
    if (this->Colors && this->ColorPerVertex)
      {
        newColors = vtkUnsignedCharArray::New();
        newColors->SetNumberOfComponents(3);
        newColors->DeepCopy(this->Colors);
      };
    if (this->Normals && this->NormalPerVertex)
       {
        newNormals = vtkFloatArray::New();
        newNormals->SetNumberOfComponents(3);
        newNormals->DeepCopy(this->Normals);
       }
    if (this->TexCoords)
       {
        newTexCoords = vtkFloatArray::New();
        newTexCoords->SetNumberOfComponents(2);
        newTexCoords->DeepCopy(this->TexCoords);
       }
    
    vtkIdType npts, *pts;
    cells->InitTraversal();

    while(cells->GetNextCell(npts, pts) != 0)
      {
      for(vtkIdType i = 0; i < npts; i++)
        {
          int vpos = pts[i];

          Vertex v;
          v.color  = colorPerVertexIndexed  ? this->ColorIndex->GetValue(position)  : -1;
          v.normal = normalPerVertexIndexed ? this->NormalIndex->GetValue(position) : -1;
          v.tc     = textureIndexed         ? this->TexCoordIndex->GetValue(position) : -1;

          VertexMap::const_iterator I = vmap.find(vpos);
          int reuse = 0;
          if (I == vmap.end()) // Vertex does not exist
            {
            vmap.insert( std::pair<int, Vertex>(vpos, v));
            }
          else if (!((*I).second == v)) // Vertex split
            {
            pts[i] = this->Coords->InsertNextPoint(this->Coords->GetPoint(vpos));
            }
          else
            {
            // Same vertex can be resused
            reuse = 1;
            }

          if(!reuse)
            {
              if(newColors)
                {
                unsigned char c[3];
                this->Colors->GetTupleValue(v.color, c);
                newColors->InsertTupleValue(pts[i], c);
                }
              if(newNormals)
                {
                float f[3];
                this->Normals->GetTupleValue(v.normal, f);
                newNormals->InsertTupleValue(pts[i], f);
                }
              if(newTexCoords)
                {
                float f[2];
                this->TexCoords->GetTupleValue(v.tc, f);
                newTexCoords->InsertTupleValue(pts[i], f);
                }
            }
          position++; vertexNr++;
        }
        position++; // skip -1
      }

    if (newColors)
      this->SetColors(newColors);
    if (newNormals)
      this->SetNormals(newNormals);
    if (newTexCoords)
      this->SetTexCoords(newTexCoords);
    }
    
  
  int calcVertexNormals = false;

  pd->SetPolys(cells);
  cells->Delete();

  pd->SetPoints(this->Coords);
  points->Delete();

  // ----
  // Start Normal processing
  // ----
  if (!this->Normals)
    {
    if (this->NormalIndex) // E3
      {
      vtkWarningMacro(<<"normalIndex given but no normals. Ignoring normalIndex (E3).");
      }
      // N1 || N2
      calcVertexNormals = this->NormalPerVertex;
    }
  else // We have a normal Array
    {
    if (!this->NormalPerVertex) // Cell normals
	    {
      if (this->NormalIndex) // N3
        {
        vtkFloatArray* resolvedNormals = vtkFloatArray::New();
        resolvedNormals->SetNumberOfComponents(3);
        for(vtkIdType i = 0; i < this->NormalIndex->GetNumberOfTuples(); i++)
          {
          vtkIdType index = this->NormalIndex->GetValue(i);
          resolvedNormals->InsertNextTuple(this->Normals->GetTuple3(index));
          }
        pd->GetCellData()->SetNormals(resolvedNormals);
        resolvedNormals->Delete();
        }
      else // N4
        {
        pd->GetCellData()->SetNormals(this->Normals);
        }
	    }
    else // Vertex normals
      {
      //assert(!this->NormalIndex); // Vertex split needed (N5)
      // N6
      pd->GetPointData()->SetNormals(this->Normals);
      }
    } // End normal processing

  // ----
  // Start Color processing
  // ----
  if(!this->Colors)
    {
    if (this->ColorIndex) // E4
      {
      vtkWarningMacro(<<"colorIndex given but no colors. Ignoring colorIndex (E4).");
      }
    // C1: Nothing to do
    }
  else
    {
    if (!this->ColorPerVertex) // Cell colors
      {
      if (this->ColorIndex) // C2
        {
        vtkUnsignedCharArray* resolvedColors = vtkUnsignedCharArray::New();
        resolvedColors->SetNumberOfComponents(3);
        unsigned char color[3];
        for(vtkIdType i = 0; i < this->ColorIndex->GetNumberOfTuples(); i++)
          {
          vtkIdType index = this->ColorIndex->GetValue(i);
          this->GetColors()->GetTupleValue(index, color);
		      resolvedColors->InsertNextTupleValue(color);
          }
        pd->GetCellData()->SetScalars(resolvedColors);
        resolvedColors->Delete();
        }
      else // C3
        {
        // C5 (the colours in the X3DColorNode node are applied to each face of the IndexedFaceSet in order)
        pd->GetCellData()->SetScalars(this->Colors);
        }
      }
    else // Vertex colors
      {
      pd->GetPointData()->SetScalars(this->Colors);
      }
     } // End normal processing

  // ----
  // Start Texture Coordinate processing
  // ----
  if (!this->TexCoords)
    {
    if (this->TexCoordIndex) // E5
      {
      vtkWarningMacro(<<"texCoordIndex given but no texture coordinates. Ignoring texCoordIndex (E5).");
      }
    // T1
    // TODO: Implement Texture Coordinate calculation
    }
  else // TexCoords are alway per vertex
    {
      assert(!this->TexCoordIndex); // Vertex split needed (T2)
      // T3
      pd->GetPointData()->SetTCoords(this->TexCoords);
    }

    /*vtkHedgeHog* normViz = vtkHedgeHog::New();
    normViz->SetVectorModeToUseNormal();
    normViz->SetInput(pd);
    normViz->Update();
    
    vtkAppendPolyData* ap = vtkAppendPolyData::New();
    ap->UserManagedInputsOff();
    ap->AddInput(pd);
    ap->AddInput(normViz->GetOutput());
    ap->Update();
    output->ShallowCopy(ap->GetOutput());*/

    if (calcVertexNormals) // N1
	  {
		  vtkPolyDataNormals* n = vtkPolyDataNormals::New();
		  n->ComputePointNormalsOn();
      n->SetFeatureAngle(vtkMath::DegreesFromRadians(this->CreaseAngle));
		  n->SetInput(pd);
		  n->Update();
		  output->ShallowCopy(n->GetOutput());
		  n->Delete();
	  }
	  else 
		  output->ShallowCopy(pd);

  pd->Delete();
  return 1;
}

void vtkX3DIndexedFaceSetSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
	os << indent << "NormalPerVertex: " 
		<< (this->NormalPerVertex ? "On\n" : "Off\n");
	os << indent << "ColorPerVertex: " 
		<< (this->ColorPerVertex ? "On\n" : "Off\n");
}

