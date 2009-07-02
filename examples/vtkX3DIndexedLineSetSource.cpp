#include "vtkX3DIndexedLineSetSource.h"

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

vtkCxxRevisionMacro(vtkX3DIndexedLineSetSource, "$Revision: 1.19 $");
vtkStandardNewMacro(vtkX3DIndexedLineSetSource);

vtkX3DIndexedLineSetSource::vtkX3DIndexedLineSetSource()
{
	this->ColorPerVertex = 1;

	this->CoordIndex = NULL;
	this->ColorIndex = NULL;

	this->Coords = NULL;
	this->Colors = NULL;

	this->SetNumberOfInputPorts(0);
}

vtkX3DIndexedLineSetSource::~vtkX3DIndexedLineSetSource()
{
	if (this->CoordIndex)
	{
		this->CoordIndex->Delete();
	}

	if (this->ColorIndex)
	{
		this->ColorIndex->Delete();
	}

	if (this->Coords)
	{
		this->Coords->Delete();
	}

	if (this->Colors)
	{
		this->Colors->Delete();
	}
}

void vtkX3DIndexedLineSetSource::SetCoordIndex(vtkIdTypeArray * i)
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

void vtkX3DIndexedLineSetSource::SetColorIndex(vtkIdTypeArray * i)
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


void vtkX3DIndexedLineSetSource::SetCoords(vtkPoints * i)
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



void vtkX3DIndexedLineSetSource::SetColors(vtkUnsignedCharArray * i)
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
int vtkX3DIndexedLineSetSource::RequestData(
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

	bool hasPointColors = this->Colors && this->ColorPerVertex;
	bool hasCellColors = this->Colors && !this->ColorPerVertex;

	// Make shure, that the index ends with an -1. This is important
	// for later processing
	if (this->CoordIndex->GetValue(this->CoordIndex->GetNumberOfTuples() -1) != -1)
		this->CoordIndex->InsertNextValue(-1);


	vtkCellArray* cells = vtkCellArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
	colors->SetNumberOfComponents(3);

	vtkPolyData* tmp = vtkPolyData::New();

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


	tmp->SetLines(cells);
	tmp->SetPoints(points);

	if (hasPointColors)
	{
		int scalars = tmp->GetPointData()->SetScalars(colors);
	}
	else if (hasCellColors)
	{
		int scalars = tmp->GetCellData()->SetScalars(colors);
	}
	output->ShallowCopy(tmp);

	tmp->Delete();
	cells->Delete();
	points->Delete();
	colors->Delete();
	return 1;
}

void vtkX3DIndexedLineSetSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
	os << indent << "ColorPerVertex: " 
		<< (this->ColorPerVertex ? "On\n" : "Off\n");
}

