#include "vtkX3DIndexedFaceSetSource.h"

#include <cassert>

#include "vtkObjectFactory.h"
#include "vtkIdTypeArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyDataNormals.h"

vtkCxxRevisionMacro(vtkX3DIndexedFaceSetSource, "$Revision: 1.19 $");
vtkStandardNewMacro(vtkX3DIndexedFaceSetSource);

vtkX3DIndexedFaceSetSource::vtkX3DIndexedFaceSetSource()
{
	this->ColorPerVertex = 1;
	this->NormalPerVertex = 1;

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
	vtkInformationVector **inputVector,
	vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!GetCoords() || GetCoords()->GetNumberOfPoints() == 0)
	{
		vtkErrorMacro(<<"No coordinates given.");
		return 1;
	}
	if (!GetCoordIndex() || GetCoordIndex()->GetNumberOfTuples() == 0)
	{
		vtkErrorMacro(<<"No coordinate index given.");
		return 1;
	}

	bool hasTextureCoordinates = this->GetTexCoords() != NULL;
	bool hasPointColors = this->Colors && this->ColorPerVertex;
	bool hasPointNormals = this->Normals && this->NormalPerVertex;
	bool hasCellColors = this->Colors && !hasPointColors;
	bool hasCellNormals = this->Normals && !hasPointNormals;
	bool calcPointNormals = !this->Normals && this->NormalPerVertex;

	// Make shure, that the index ends with an -1. This is important
	// for later processing
	if (this->CoordIndex->GetValue(this->CoordIndex->GetNumberOfTuples() -1) != -1)
		this->CoordIndex->InsertNextValue(-1);


	vtkCellArray* cells = vtkCellArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray* texCoords = vtkFloatArray::New();
	vtkFloatArray* normals = vtkFloatArray::New();
	vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
	vtkPolyData* tmp = vtkPolyData::New();

	texCoords->SetNumberOfComponents(2);
	normals->SetNumberOfComponents(3);
	colors->SetNumberOfComponents(3);

	int indexPos = 0;
	int nrVertex = 0;
	int counter = 0;

	vtkIdType* pIndex1 = this->CoordIndex->GetPointer(0);
	
	int indexCount = this->CoordIndex->GetNumberOfTuples();
	pIndex1 = GetCoordIndex()->GetPointer(0);

	for(int i = 0; i<indexCount; i++)
	{
		vtkIdType coordIndex = *pIndex1++;
		if (coordIndex == -1)  // The -1 splits the cells
		{
			cells->InsertNextCell(counter);
			for (int j = counter; j > 0; j--)
			{
				cells->InsertCellPoint(nrVertex -j);
			}
			counter = 0;
			indexPos++;
			continue;
		}

		// Vertex and vertex position
		assert(this->GetCoords()->GetNumberOfPoints() > coordIndex);
		double* pos = this->GetCoords()->GetPoint(coordIndex);
		points->InsertNextPoint(pos);

		// Normals
		if (hasPointNormals)
		{
			int ni = coordIndex;
			if (this->GetNormalIndex())
			{
				ni = this->GetNormalIndex()->GetValue(indexPos);
			}
			assert(GetNormals()->GetNumberOfTuples() > ni);
			float normal[3];
			this->GetNormals()->GetTupleValue(ni, normal);
			normals->InsertNextTupleValue(normal);
		}

		// Colors
		if (hasPointColors)
		{
			int ni = coordIndex;
			if (this->GetColorIndex())
			{
				ni = this->GetColorIndex()->GetValue(indexPos);
			}
			assert(GetColors()->GetNumberOfTuples() > ni);
			unsigned char color[3];
			this->GetColors()->GetTupleValue(ni, color);
			colors->InsertNextTupleValue(color);
		}

		// TexCoords
		if (hasTextureCoordinates)
		{
			int ti = coordIndex;
			if (this->GetTexCoordIndex())
			{
				ti = this->GetTexCoordIndex()->GetValue(indexPos);
			}
			assert(GetTexCoords()->GetNumberOfTuples() > ti);
			double* texCoord = this->GetTexCoords()->GetTuple2(ti);
			texCoords->InsertNextTuple2(texCoord[0], 1.0 - texCoord[1]);
		}

		nrVertex++;
		counter++;
		indexPos++;
	}

	int cellCount = cells->GetNumberOfCells();

	if (hasCellColors)
	{
		unsigned char color[3];
		if (this->GetColorIndex())
		{
			assert(cellCount = this->GetColorIndex()->GetNumberOfTuples());
			for (int i = 0; i < cellCount; i++)
			{
				this->GetColors()->GetTupleValue(this->GetColorIndex()->GetValue(i), color);
				colors->InsertNextTupleValue(color);
			}
		}
		else
		{
			assert(cellCount <= this->GetColors()->GetNumberOfTuples());
			for (int i = 0; i < cellCount; i++)
			{
				this->GetColors()->GetTupleValue(i, color);
				colors->InsertNextTupleValue(color);
			}	
		}
	}

	if (hasCellNormals)
	{
		float normal[3];
		if (this->GetNormalIndex())
		{
			assert(cellCount == this->GetNormalIndex()->GetNumberOfTuples());
			for (int i = 0; i < cellCount; i++)
			{
				this->GetNormals()->GetTupleValue(this->GetNormalIndex()->GetValue(i), normal);
				normals->InsertNextTupleValue(normal);
			}
		}
		else
		{
			assert(cellCount <= this->GetNormals()->GetNumberOfTuples());
			for (int i = 0; i < cellCount; i++)
			{
				this->GetNormals()->GetTupleValue(i, normal);
				normals->InsertNextTupleValue(normal);
			}	
		}
	}

	tmp->SetPolys(cells);
	tmp->SetPoints(points);

	if (hasTextureCoordinates)
		tmp->GetPointData()->SetTCoords(texCoords);

	if (hasPointNormals)
		tmp->GetPointData()->SetNormals(normals);
	else if (hasCellNormals)
		tmp->GetCellData()->SetNormals(normals);

	if (hasPointColors)
	{
		int scalars = tmp->GetPointData()->SetScalars(colors);
		//mapper->SetScalarModeToUsePointData();
		//mapper->SelectColorArray(scalars);
	}
	else if (hasCellColors)
	{
		int scalars = tmp->GetCellData()->SetScalars(colors);
		//mapper->SetScalarModeToUseCellData();
		//mapper->SelectColorArray(scalars);
	}

	

	if (calcPointNormals)
	{
		vtkPolyDataNormals* n = vtkPolyDataNormals::New();
		n->ComputePointNormalsOn();
		n->SetInput(tmp);
		n->Update();
		output->ShallowCopy(n->GetOutput());
		n->Delete();
	}
	else
		output->ShallowCopy(tmp);


	tmp->Delete();
	cells->Delete();
	points->Delete();
	texCoords->Delete();
	normals->Delete();
	colors->Delete();
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

