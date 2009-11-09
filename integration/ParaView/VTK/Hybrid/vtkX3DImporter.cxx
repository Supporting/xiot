#include "vtkX3DImporter.h"
#include "vtkObjectFactory.h"

#include <vtkstd/set>
#include <cassert>

#include <xiot/X3DLoader.h>
#include <xiot/X3DAttributes.h>
#include <xiot/X3DParseException.h>
#include <xiot/X3DDefaultNodeHandler.h>

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkTransform.h"
#include "vtkPoints.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"
#include "vtkLookupTable.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyDataNormals.h"
#include "vtkMath.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkSmartPointer.h"
#include "vtkImageReader2.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageData.h"

using namespace XIOT;


/**
 * Generate vtkPolyData from X3D IndexedFaceSet / IndexedLineSet
 *
 * Helper class to generate a vtkPolyData structure from 
 * an IndexedFaceSet or IndexedLineSet data structure. This class converts index
 * lists and resolves multiple used coordinate data.
 * @see <A href="http://www.vtk.org/doc/nightly/html/classvtkPolyDataAlgorithm.html"/>vtkPolyDataAlgorithm</A>
 * @see <A href="http://www.web3d.org/x3d/specifications/ISO-IEC-FDIS-19775-1.2-X3D-AbstractSpecification/Part01/components/geometry3D.html#IndexedFaceSet"/>IndexedFaceSet</A>
 * @ingroup vtkX3DImporter
 */
class vtkX3DIndexedGeometrySource : public vtkPolyDataAlgorithm
{
public:
  static vtkX3DIndexedGeometrySource *New();
  vtkTypeRevisionMacro(vtkX3DIndexedGeometrySource,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

//BTX
enum GeometryFormatEnum
  {
  INDEXED_FACESET,
  INDEXED_LINESET,
  };
//ETX

  vtkSetClampMacro(GeometryFormat, int, INDEXED_FACESET, INDEXED_LINESET);
  vtkGetMacro(GeometryFormat, int);
  void SetGeometryFormatToIndexedFaceSet()
    {this->SetGeometryFormat(INDEXED_FACESET);};
  void SetGeometryFormatToIndexedLineSet()
    {this->SetGeometryFormat(INDEXED_LINESET);};



  vtkSetMacro(NormalPerVertex  ,int);
  vtkGetMacro(NormalPerVertex ,int);
  vtkBooleanMacro(NormalPerVertex ,int);

  vtkSetMacro(ColorPerVertex ,int);
  vtkGetMacro(ColorPerVertex,int);
  vtkBooleanMacro(ColorPerVertex,int);

  vtkSetMacro(CreaseAngle, double);
  vtkGetMacro(CreaseAngle, double);

  vtkSetMacro(CalculateNormals  ,int);
  vtkGetMacro(CalculateNormals ,int);
  vtkBooleanMacro(CalculateNormals ,int);


  // Indices

  // Description:
  // Set the coord index array defining vertices.
  // This is in X3D format with -1 splitting cells
  void SetCoordIndex(vtkIdTypeArray * v);
  vtkIdTypeArray *GetCoordIndex() { return this->CoordIndex; };
  
  void SetNormalIndex(vtkIdTypeArray * v);
  vtkIdTypeArray *GetNormalIndex() { return this->NormalIndex; };

  void SetColorIndex(vtkIdTypeArray * v);
  vtkIdTypeArray *GetColorIndex() { return this->ColorIndex; };

  void SetTexCoordIndex(vtkIdTypeArray * v);
  vtkIdTypeArray *GetTexCoordIndex() { return this->TexCoordIndex; };

  // Data
  void SetCoords(vtkPoints *p);
  vtkPoints* GetCoords() { return this->Coords; };

  void SetNormals(vtkFloatArray* p);
  vtkFloatArray* GetNormals() { return this->Normals; };

  void SetTexCoords(vtkFloatArray* p);
  vtkFloatArray* GetTexCoords(){ return this->TexCoords; };

  void SetColors(vtkUnsignedCharArray* p);
  vtkUnsignedCharArray* GetColors(){ return this->Colors; };
  

protected:
  vtkX3DIndexedGeometrySource();
  ~vtkX3DIndexedGeometrySource();

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int processIndexedFaceSet(vtkPolyData* pd, vtkCellArray* cells);
  int processIndexedLineSet(vtkPolyData* pd, vtkCellArray* cells);
  
  void ProcessColor(vtkPolyData* pd);

  int GeometryFormat;

  int NormalPerVertex;
  int ColorPerVertex;
  int CalculateNormals;

  double CreaseAngle;

  vtkIdTypeArray* CoordIndex;
  vtkIdTypeArray* NormalIndex;
  vtkIdTypeArray* ColorIndex;
  vtkIdTypeArray* TexCoordIndex;

  vtkPoints *Coords;
  vtkFloatArray *Normals;
  vtkFloatArray *TexCoords;
  vtkUnsignedCharArray* Colors;


private:


  vtkX3DIndexedGeometrySource(const vtkX3DIndexedGeometrySource&); // Not implemented.
  void operator=(const vtkX3DIndexedGeometrySource&); // Not implemented.
};

vtkCxxRevisionMacro(vtkX3DIndexedGeometrySource, "$Revision: 1.19 $");
vtkStandardNewMacro(vtkX3DIndexedGeometrySource);

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

vtkX3DIndexedGeometrySource::vtkX3DIndexedGeometrySource()
{
  this->GeometryFormat = INDEXED_FACESET;
	
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

  this->CalculateNormals = 0;
	this->SetNumberOfInputPorts(0);
}

vtkX3DIndexedGeometrySource::~vtkX3DIndexedGeometrySource()
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

void vtkX3DIndexedGeometrySource::SetCoordIndex(vtkIdTypeArray * i)
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

void vtkX3DIndexedGeometrySource::SetNormalIndex(vtkIdTypeArray * i)
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

void vtkX3DIndexedGeometrySource::SetColorIndex(vtkIdTypeArray * i)
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

void vtkX3DIndexedGeometrySource::SetTexCoordIndex(vtkIdTypeArray * i)
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

void vtkX3DIndexedGeometrySource::SetCoords(vtkPoints * i)
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

void vtkX3DIndexedGeometrySource::SetNormals(vtkFloatArray * i)
{
	if ( i != this->Normals)
	{
		if (this->Normals)
		{
			this->Normals->UnRegister(this);
		}
		this->Normals = i;
		if (this->Normals)
		{
    	if (this->Normals->GetNumberOfComponents() != 3)
	      {
		    vtkErrorMacro(<<"Number of components for Normals must be 3");
        this->Normals = NULL;
    	  }
      else
			  this->Normals->Register(this);

		}
		this->Modified();
	}
}

void vtkX3DIndexedGeometrySource::SetTexCoords(vtkFloatArray * i)
  {
  if ( i != this->TexCoords)
    {
    if (this->TexCoords)
      {
      this->TexCoords->UnRegister(this);
      }
    this->TexCoords = i;
    if (this->TexCoords)
      {
      if (this->TexCoords->GetNumberOfComponents() != 2)
        {
        vtkErrorMacro(<<"Number of components for TexCoords must be 2");
        this->TexCoords = NULL;
        }
      else
        this->TexCoords->Register(this);
      }
    this->Modified();
    }
  }

void vtkX3DIndexedGeometrySource::SetColors(vtkUnsignedCharArray * i)
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
int vtkX3DIndexedGeometrySource::RequestData(
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

	// Create PolyData object and set vertex positions
	vtkPolyData* pd = vtkPolyData::New();
  pd->SetPoints(this->Coords); 

  // ----
  // Start Vertex position and index processing
  // ----
  vtkCellArray* cells = vtkCellArray::New();

  // Convert X3D Index format to CellArray
  int faceSize = 0, faceCount = 0;
  vtkIdType* index = this->CoordIndex->GetPointer(0);
  vtkIdType* faceStart = index;
  const vtkIdType* end = this->CoordIndex->GetPointer(this->CoordIndex->GetNumberOfTuples());
  while(index != end)
    {
      while(*index != -1)
        {
        index++; faceSize++;
        }
      faceCount++;
      if (faceSize == 0)
        {
        vtkWarningMacro(<<"Face " << faceCount << " has no vertices. Skipping.");
        }
      else
        {
        cells->InsertNextCell(faceSize);
        while(faceStart != index)
          {
          cells->InsertCellPoint(*faceStart);
          faceStart++;
          }
        }
      faceSize = 0;
      index++; faceStart++;
    }

  int success = 0;
  switch(this->GeometryFormat)
    {
    case INDEXED_FACESET:
      {
      success = processIndexedFaceSet(pd, cells);
      break;
      }
    case INDEXED_LINESET:
      {
      success = processIndexedLineSet(pd, cells);
      break;
      }
    default:
      vtkErrorMacro(<<"Unknown geometry format: " << this->GeometryFormat);
    }

    if (success)
      output->ShallowCopy(pd);
    pd->Delete();
    
    return success;
  }

int vtkX3DIndexedGeometrySource::processIndexedFaceSet(vtkPolyData* pd, vtkCellArray* cells)
  {
  bool normalPerVertexIndexed = this->Normals && this->NormalPerVertex &&  this->NormalIndex;
  bool colorPerVertexIndexed = this->Colors && this->ColorPerVertex &&  this->ColorIndex;
  bool textureIndexed = this->TexCoords && this->TexCoordIndex;

  // Use cell data as polygons
  pd->SetPolys(cells);
  cells->Delete();

  // Is vertex split needed or can we use data as is?
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
            vmap.insert( std::pair<int, Vertex>(pts[i], v));
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
      {
      this->SetColors(newColors);
      newColors->Delete();
      }
    if (newNormals)
      {
      this->SetNormals(newNormals);
      newNormals->Delete();
      }
    if (newTexCoords)
      {
      this->SetTexCoords(newTexCoords);
      newTexCoords->Delete();
      }
    }
    
  int calcVertexNormals = false;
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
  // Color processing
  // ----
  this->ProcessColor(pd);

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
      // assert(!this->TexCoordIndex); // Vertex split needed (T2)
      // T3
      pd->GetPointData()->SetTCoords(this->TexCoords);
    }

  if (calcVertexNormals && this->CalculateNormals) // N1
	  {
      vtkDebugMacro(<< "Calculating Vertex Normals.");
		  vtkPolyDataNormals* n = vtkPolyDataNormals::New();
		  n->ComputePointNormalsOn();
      n->SetFeatureAngle(this->CreaseAngle);
		  n->SetInput(pd);
		  n->Update();
		  pd->ShallowCopy(n->GetOutput());
		  n->Delete();
	  }
  return 1;
}

int vtkX3DIndexedGeometrySource::processIndexedLineSet(vtkPolyData* pd, vtkCellArray* cells)
  {
  bool colorPerVertexIndexed = this->Colors && this->ColorPerVertex &&  this->ColorIndex;

  // Use cell data as polygons
  pd->SetLines(cells);
  cells->Delete();

 // Is vertex split needed or can we use data as is?
	if(colorPerVertexIndexed) 
    {
    int position = 0, vertexNr = 0;
    VertexMap vmap;
    vtkUnsignedCharArray* newColors = vtkUnsignedCharArray::New();
    newColors->SetNumberOfComponents(3);
    newColors->DeepCopy(this->Colors);

    vtkIdType npts, *pts;
    cells->InitTraversal();

    while(cells->GetNextCell(npts, pts) != 0)
      {
      for(vtkIdType i = 0; i < npts; i++)
        {
          int vpos = pts[i];

          Vertex v;
          v.color  = this->ColorIndex->GetValue(position);

          VertexMap::const_iterator I = vmap.find(vpos);
          int reuse = 0;
          if (I == vmap.end()) // Vertex does not exist
            {
            vmap.insert( std::pair<int, Vertex>(vpos, v));
            }
          else if (!((*I).second == v)) // Vertex split
            {
            pts[i] = this->Coords->InsertNextPoint(this->Coords->GetPoint(vpos));
            vmap.insert( std::pair<int, Vertex>(pts[i], v));
            }
          else
            {
            // Same vertex can be resused
            reuse = 1;
            }

          if(!reuse)
            {
              unsigned char c[3];
              this->Colors->GetTupleValue(v.color, c);
              newColors->InsertTupleValue(pts[i], c);
            }
          position++; vertexNr++;
        }
        position++; // skip -1
      }

      this->SetColors(newColors);
      newColors->Delete();
    
    } // End vertex split
   
  this->ProcessColor(pd);
  return 1;
  }

// ----
// Start Color processing
// ----
void vtkX3DIndexedGeometrySource::ProcessColor(vtkPolyData* pd)
  {
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
    } 
  } // End ProcessColor

void vtkX3DIndexedGeometrySource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
	os << indent << "NormalPerVertex: " 
		<< (this->NormalPerVertex ? "On\n" : "Off\n");
	os << indent << "ColorPerVertex: " 
		<< (this->ColorPerVertex ? "On\n" : "Off\n");
}


/**
 * Handler that fills a vtkRenderer from X3D callbacks.
 *
 * The vtkX3DNodeHandler derives from the X3DDefaultNodeHandler. It overrides a selection of the callback functions
 * and is used to import a X3D file into a vtk-application. To construct the scene, the class provides an overloaded
 * constructor which is used to save a pointer to a vtkRenderer object.
 * @see X3DDefaultNodeHandler
 * @ingroup vtkX3DImporter
 */
class vtkX3DNodeHandler : public XIOT::X3DDefaultNodeHandler
{
public:
	
  vtkX3DNodeHandler(vtkRenderer* Renderer, vtkX3DImporter* Importer);
	~vtkX3DNodeHandler();

  void startDocument();

	int startUnhandled(const char* nodeName, const XIOT::X3DAttributes &attr);
	
	int startAppearance(const XIOT::X3DAttributes &attr);
	
	int startMaterial(const XIOT::X3DAttributes &attr);
	
	int startTransform(const XIOT::X3DAttributes &attr);
	int endTransform();

	int startSphere(const XIOT::X3DAttributes &attr);

	int startShape(const XIOT::X3DAttributes &attr);
	int endShape();

	int startIndexedLineSet(const XIOT::X3DAttributes &attr);
	int endIndexedLineSet();

	int startIndexedFaceSet(const XIOT::X3DAttributes &attr);
	int endIndexedFaceSet();


	int startCoordinate(const XIOT::X3DAttributes &attr);
	
	int startNormal(const XIOT::X3DAttributes &attr);
	
	int startColor(const XIOT::X3DAttributes &attr);
	
	int startTextureCoordinate(const XIOT::X3DAttributes &attr);
	
	int startPointSet(const XIOT::X3DAttributes &attr);
	int endPointSet();

	int startBox(const XIOT::X3DAttributes &attr);

	int startCone(const XIOT::X3DAttributes &attr);

	int startCylinder(const XIOT::X3DAttributes &attr);
	
	int startDirectionalLight(const XIOT::X3DAttributes &attr);
	
  /// Textures
  int startPixelTexture(const XIOT::X3DAttributes &attr);
  int startImageTexture(const XIOT::X3DAttributes &attr);

  /// X3DBindables
  int startNavigationInfo(const XIOT::X3DAttributes &attr);
	int startBackground(const XIOT::X3DAttributes &attr);
  int startViewpoint(const XIOT::X3DAttributes &attr);

protected:
  template<class T>
  int checkReferencing(const XIOT::X3DAttributes &attr, T** obj, bool shouldDelete) {
    if (attr.isUSE())
      {
      vtkObject* p = this->DefMap[attr.getUSE()];
      if (shouldDelete)
        {
        (*obj)->Delete();
        }
      *obj = T::SafeDownCast(p);
      if(!obj)
        {
        vtkWarningWithObjectMacro(this->Importer, << "Could not find node of type <" << (*obj)->GetClassName() << "> with DEF=\"" << attr.getUSE() << "\"."); \
        }
      this->IsCurrentUSE = true;
      return true;
      }
    this->IsCurrentUSE = false;
    if (attr.isDEF())
      {
      (*obj)->Register(this->MapReferencer); // Reference count array
      this->DefMap[attr.getDEF()] = *obj;
      }
    return false;
    }

private:
	vtkRenderer			*Renderer;
  vtkX3DImporter  *Importer;
  vtkLight        *HeadLight;

	vtkActor                    *CurrentActor;
	vtkTransform                *CurrentTransform;
	vtkPoints                   *CurrentPoints;
	vtkFloatArray               *CurrentNormals;
	vtkFloatArray               *CurrentTCoords;
	vtkUnsignedCharArray        *CurrentColors;
	vtkX3DIndexedGeometrySource *CurrentIndexedGeometry;
  
  int IsCurrentUnlit;
  int IsCurrentUSE;

  double CurrentEmissiveColor[3];

  int SeenBindables;

  vtkObject *MapReferencer;

	std::map<std::string,vtkObject*>			DefMap;		// used for DEF/USE
	std::set<int> _ignoreNodes;

};

#define checkInShape(kind) \
  if(!this->CurrentActor) \
    { \
    vtkWarningWithObjectMacro(this->Importer, << "Ingoring <" << #kind << "> found outside of <Shape>."); \
    return SKIP_CHILDREN; \
    } 

#define X3D_BACKGROUND      1
#define X3D_VIEWPOINT       2
#define X3D_NAVIGATIONINFO  4

vtkX3DNodeHandler::vtkX3DNodeHandler(vtkRenderer* Renderer, vtkX3DImporter * Importer)
  {
  this->Renderer = Renderer;
  this->Importer = Importer;

  this->CurrentActor = NULL;
  this->CurrentPoints = NULL;
  this->CurrentNormals = NULL;
  this->CurrentTCoords = NULL;
  this->CurrentIndexedGeometry = NULL;
  this->CurrentColors = NULL;
  this->CurrentTransform = vtkTransform::New();
  this->HeadLight = NULL;

  this->MapReferencer = vtkObject::New();
  this->IsCurrentUnlit = 0;
  this->IsCurrentUSE = 0;

  _ignoreNodes.insert(ID::X3D);
  _ignoreNodes.insert(ID::Scene);
  _ignoreNodes.insert(ID::StaticGroup);
  _ignoreNodes.insert(ID::Group);
  _ignoreNodes.insert(ID::Switch);
  }

vtkX3DNodeHandler::~vtkX3DNodeHandler()
  {
  assert(!this->CurrentActor); // Should be deleted in endShape
  assert(!this->CurrentPoints); // Should be deleted in endShape
  assert(!this->CurrentNormals); // Should be deleted in endShape
  assert(!this->CurrentTCoords); // Should be deleted in endShape
  assert(!this->CurrentColors); // Should be deleted in endShape
  // Should be deleted in 
  // endIndexedFaceSet/endIndexedLineSet
  assert(!this->CurrentIndexedGeometry); 
  
  if (this->HeadLight)
    {
    this->HeadLight->Delete();
    }
  this->CurrentTransform->Delete();

  // clean up saved references
  for(std::map<std::string, vtkObject*>::iterator I = this->DefMap.begin(); I != this->DefMap.end(); I++)
    {
    I->second->UnRegister(this->MapReferencer);
    }
  this->MapReferencer->Delete();
  }


void vtkX3DNodeHandler::startDocument()
  {
  // Set some default values
  this->Renderer->SetBackground(0.0, 0.0, 0.0);

  this->Renderer->SetAmbient(0.0, 0.0, 0.0);
	this->Renderer->SetLightFollowCamera(1);

  // The Default NavigationInfo adds an headlight
  this->HeadLight = vtkLight::New();
	this->HeadLight->SetLightTypeToHeadlight();
	this->HeadLight->SetColor(1.0, 1.0, 1.0);
	this->HeadLight->SetIntensity(1.0);
	this->Renderer->AddLight(this->HeadLight);

  this->SeenBindables = 0;
  }


int vtkX3DNodeHandler::startTransform(const X3DAttributes &attr)
  {
  this->CurrentTransform->Push();
  // No defaults needed, as vtk uses the same defaults for the translation, rotation, scale
  // Check for translation
  int index = attr.getAttributeIndex(ID::translation);
  if(index != -1)
    {
    SFVec3f vec;
    attr.getSFVec3f(index, vec);
    this->CurrentTransform->Translate(vec.x, vec.y, vec.z);
    }

  // Check for rotation
  index = attr.getAttributeIndex(ID::rotation);
  if(index != -1)
    {
    SFRotation rot;
    attr.getSFRotation(index, rot);
    this->CurrentTransform->RotateWXYZ(vtkMath::DegreesFromRadians(rot.angle), &rot.x);
    }

  // Check for scale
  index = attr.getAttributeIndex(ID::scale);
  if(index != -1)
    {
    SFVec3f vec;
    attr.getSFVec3f(index, vec);
    this->CurrentTransform->Scale(vec.x, vec.y, vec.z);
    }

  return CONTINUE;
  }

int vtkX3DNodeHandler::endTransform()
  {
  this->CurrentTransform->Pop();
  return CONTINUE;
  }

int vtkX3DNodeHandler::startMaterial(const X3DAttributes &attr)
  {
  bool ambientSet = false;

  checkInShape(Material);

  vtkProperty* prop = this->CurrentActor->GetProperty();
  if (checkReferencing(attr, &prop, false))
    {
    if (!prop)
      return SKIP_CHILDREN;

    vtkDebugWithObjectMacro(this->Importer, << " Reusing Material " << attr.getUSE());
    this->CurrentActor->SetProperty(prop);
    return CONTINUE;
    }

  prop->LightingOn();
  // Check for ambientIntensity
  int index = attr.getAttributeIndex(ID::ambientIntensity);
  if(index != -1)
    {
    float ambientIntensity = attr.getSFFloat(index);
    prop->SetAmbient(ambientIntensity);
    ambientSet = true;
    }
  else
    prop->SetAmbient(0.2f);

  // Check for diffuse color
  index = attr.getAttributeIndex(ID::diffuseColor);
  if(index != -1)
    {
    SFColor col;
    attr.getSFColor(index, col);
    // Set both, ambient and diffuse Color to this value
    // The ambient Color will be multiplied with the ambientIntensity
    // above
    prop->SetDiffuseColor(col.r, col.g, col.b);
    prop->SetAmbientColor(col.r, col.g, col.b);
    }
  else
    {
    prop->SetDiffuseColor(0.8f, 0.8f, 0.8f);
    prop->SetAmbientColor(0.8f, 0.8f, 0.8f);
    }

  // Check for emissiveColor
  // VTK does not support emissive color. It will be approximated
  // using ambient color only if ambientIntensity is not set.
  index = attr.getAttributeIndex(ID::emissiveColor);
  if(index != -1)
    {
    SFColor col;
    attr.getSFColor(index, col);
    if (!ambientSet)
      {
      prop->SetAmbientColor(col.r, col.g, col.b);
      prop->SetAmbient(1.0);
      }
    this->CurrentEmissiveColor[0] = col.r;
    this->CurrentEmissiveColor[1] = col.g;
    this->CurrentEmissiveColor[2] = col.b;
    }

  // Check for shininess
  index = attr.getAttributeIndex(ID::shininess);
  if(index != -1)
    {
    float shininess = attr.getSFFloat(index);
    // The X3D range for shininess is [0,1] while for OpenGL is [0,128]
    prop->SetSpecularPower(shininess * 128.0);
    }
  else
    prop->SetSpecularPower(0.2 * 128.0);

  // Check for specularColor
  index = attr.getAttributeIndex(ID::specularColor);
  if(index != -1)
    {
    SFColor col;
    attr.getSFColor(index, col);
    prop->SetSpecularColor(col.r, col.g, col.b);
    }
  else
    prop->SetSpecularColor(0.0f, 0.0f, 0.0f);

  // Check for transparency
  index = attr.getAttributeIndex(ID::transparency);
  if(index != -1)
    {
    float transparency = attr.getSFFloat(index);
    prop->SetOpacity(1.0f - transparency);
    }
  else
    prop->SetOpacity(1.0f);

  return CONTINUE;
  }

int vtkX3DNodeHandler::startAppearance(const X3DAttributes &attr)
  {
  checkInShape(Appearance);
  vtkProperty* p = this->CurrentActor->GetProperty();
 
  this->CurrentEmissiveColor[0] = 0.0;
  this->CurrentEmissiveColor[1] = 0.0;
  this->CurrentEmissiveColor[2] = 0.0;
 
  this->IsCurrentUnlit = 0;

  if (checkReferencing(attr, &p, false))
    {
      if (!p)
        return SKIP_CHILDREN;
      vtkDebugWithObjectMacro(this->Importer, << " Reusing Appearance " << attr.getUSE());
      this->CurrentActor->SetProperty(p);
      return CONTINUE;
    }
  this->CurrentActor->GetProperty()->SetInterpolationToPhong();
  return CONTINUE;
  }

int vtkX3DNodeHandler::startShape(const X3DAttributes &attr)
  {
  this->CurrentActor = vtkActor::New();
  // This saves the color for unlit geometry
  this->CurrentEmissiveColor[0] = this->CurrentEmissiveColor[1] = this->CurrentEmissiveColor[2] = 1.0;
  // By default, everything is unlit
  this->IsCurrentUnlit = 1;

  // Check for DEF
  if (checkReferencing(attr, &this->CurrentActor, true))
    {
      if(!this->CurrentActor)
        return SKIP_CHILDREN;
      
      vtkActor* actor = vtkActor::New();
      actor->ShallowCopy(this->CurrentActor);
      this->CurrentActor = actor;
    }
  else
    {
    // Lighting is off by default. An occurence of a Material
    // node will enable lighting
    this->CurrentActor->GetProperty()->LightingOff();
    }

  // This overrides the transformation derived by the "ShallowCopy(..)"
  this->CurrentActor->SetOrientation(this->CurrentTransform->GetOrientation());
  this->CurrentActor->SetPosition(this->CurrentTransform->GetPosition());
  this->CurrentActor->SetScale(this->CurrentTransform->GetScale());

  // Add actor to renderer
  this->Renderer->AddActor(this->CurrentActor);
  return CONTINUE;
  }

int vtkX3DNodeHandler::endShape()
  {

  if (this->IsCurrentUnlit)
    this->CurrentActor->GetProperty()->SetColor(this->CurrentEmissiveColor);

  // Delete global geometry data
  if (this->CurrentPoints)
    {
    this->CurrentPoints->Delete();
    this->CurrentPoints = NULL;
    }
  if (this->CurrentColors)
    {
    this->CurrentColors->Delete();
    this->CurrentColors = NULL;
    }
  if (this->CurrentTCoords)
    {
    this->CurrentTCoords->Delete();
    this->CurrentTCoords = NULL;
    }
  if (this->CurrentNormals)
    {
    this->CurrentNormals->Delete();
    this->CurrentNormals = NULL;
    }
  if (this->CurrentActor)
    {
    this->CurrentActor->Delete();
    this->CurrentActor = NULL;
    }

  return CONTINUE;
  }

int vtkX3DNodeHandler::startSphere(const X3DAttributes &attr)
  {
  vtkSmartPointer<vtkPolyDataMapper> pmap = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();

  checkInShape(Sphere);

  // Check for radius
  int index = attr.getAttributeIndex(ID::radius);
  if(index != -1)
    {
    float fRadius = attr.getSFFloat(index);
    sphere->SetRadius(fRadius);
    }
  else
    sphere->SetRadius(1.0f);

  sphere->SetThetaResolution(20);
  sphere->SetPhiResolution(20);
  pmap->SetInput(sphere->GetOutput());
  this->CurrentActor->SetMapper(pmap);
  return CONTINUE;
  }


int vtkX3DNodeHandler::startIndexedFaceSet(const X3DAttributes &attr) 
  {
  checkInShape(IndexedFaceSet);

  vtkPolyDataMapper* pmap = vtkPolyDataMapper::New();
  if (checkReferencing(attr, &pmap, true))
    {
    this->CurrentActor->SetMapper(pmap);
    return CONTINUE;
    }
  this->CurrentActor->SetMapper(pmap);
  pmap->Delete();

  this->CurrentIndexedGeometry = vtkX3DIndexedGeometrySource::New();
  this->CurrentIndexedGeometry->SetGeometryFormatToIndexedFaceSet();
  this->CurrentIndexedGeometry->SetDebug(this->Importer->GetDebug());
  this->CurrentIndexedGeometry->SetCalculateNormals(this->Importer->GetCalculateNormals());

  // normal per vertex
  int index = attr.getAttributeIndex(ID::normalPerVertex);
  if(index != -1 && !attr.getSFBool(index))
    {
    this->CurrentIndexedGeometry->NormalPerVertexOff();
    }
  else
    this->CurrentIndexedGeometry->NormalPerVertexOn();

  // color per vertex
  index = attr.getAttributeIndex(ID::colorPerVertex);
  if(index != -1 && !attr.getSFBool(index))
    {
    this->CurrentIndexedGeometry->ColorPerVertexOff();
    }
  else
    this->CurrentIndexedGeometry->ColorPerVertexOn();

  // coord index
  index = attr.getAttributeIndex(ID::coordIndex);
  if(index != -1)
    {
    MFInt32 coords;
    attr.getMFInt32(index, coords);
    vtkSmartPointer<vtkIdTypeArray> idArray = vtkSmartPointer<vtkIdTypeArray>::New();
    for(std::vector<int>::iterator I = coords.begin(); I != coords.end(); I++)
      {
      idArray->InsertNextValue(*I);
      }
    this->CurrentIndexedGeometry->SetCoordIndex(idArray);
    }

  // color index
  index = attr.getAttributeIndex(ID::colorIndex);
  if(index != -1)
    {
    MFInt32 coords;
    attr.getMFInt32(index, coords);
    vtkSmartPointer<vtkIdTypeArray> idArray = vtkSmartPointer<vtkIdTypeArray>::New();
    for(std::vector<int>::iterator I = coords.begin(); I != coords.end(); I++)
      {
      idArray->InsertNextValue(*I);
      }
    this->CurrentIndexedGeometry->SetColorIndex(idArray);
    }

  // normal index
  index = attr.getAttributeIndex(ID::normalIndex);
  if(index != -1)
    {
    MFInt32 coords;
    attr.getMFInt32(index, coords);
    vtkSmartPointer<vtkIdTypeArray> idArray = vtkSmartPointer<vtkIdTypeArray>::New();
    for(std::vector<int>::iterator I = coords.begin(); I != coords.end(); I++)
      {
      idArray->InsertNextValue(*I);
      }
    this->CurrentIndexedGeometry->SetNormalIndex(idArray);
    }

  // texCoord index
  index = attr.getAttributeIndex(ID::texCoordIndex);
  if(index != -1)
    {
    MFInt32 coords;
    attr.getMFInt32(index, coords);
    vtkSmartPointer<vtkIdTypeArray> idArray = vtkSmartPointer<vtkIdTypeArray>::New();
    for(std::vector<int>::iterator I = coords.begin(); I != coords.end(); I++)
      {
      idArray->InsertNextValue(*I);
      }
    this->CurrentIndexedGeometry->SetTexCoordIndex(idArray);
    }

  index = attr.getAttributeIndex(ID::creaseAngle);
  if (index != -1)
    {
    this->CurrentIndexedGeometry->SetCreaseAngle(vtkMath::DegreesFromRadians(attr.getSFFloat(index)));
    }
  else
    this->CurrentIndexedGeometry->SetCreaseAngle(0.0);

  return CONTINUE;
  }

int vtkX3DNodeHandler::endIndexedFaceSet()
  {
  if (this->IsCurrentUSE)
    return CONTINUE;

  vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();

  this->CurrentIndexedGeometry->SetCoords(this->CurrentPoints);
  this->CurrentIndexedGeometry->SetColors(this->CurrentColors);
  this->CurrentIndexedGeometry->SetTexCoords(this->CurrentTCoords);
  this->CurrentIndexedGeometry->SetNormals(this->CurrentNormals);
  this->CurrentIndexedGeometry->Update();
  
  // This is to disconnect the geometry from the source  
  pd->ShallowCopy(this->CurrentIndexedGeometry->GetOutput());
  ((vtkPolyDataMapper*)this->CurrentActor->GetMapper())->SetInput(pd);

  vtkDebugWithObjectMacro(this->Importer, 
    << "Generated IndexedFaceSet with " << pd->GetPolys()->GetNumberOfCells() << " faces, "
    << pd->GetNumberOfPoints() << " points and "
    << (      pd->GetPointData()->GetNormals() ? pd->GetPointData()->GetNormals()->GetNumberOfTuples() 
           :  pd->GetCellData()->GetNormals()  ? pd->GetCellData()->GetNormals()->GetNumberOfTuples()
           :  0) 
    << " normals.");
    
  this->CurrentIndexedGeometry->Delete();
  this->CurrentIndexedGeometry = NULL;
  return CONTINUE;
  }

int vtkX3DNodeHandler::startCoordinate(const X3DAttributes &attr) 
  {
  this->CurrentPoints = vtkPoints::New();
  if (checkReferencing(attr, &this->CurrentPoints, true))
    {
      if (!this->CurrentPoints)
        return SKIP_CHILDREN;

      this->CurrentPoints->Register(NULL);
      return CONTINUE;
    }

  int index = attr.getAttributeIndex(ID::point);
  if(index != -1)
    {
    MFVec3f coords;
    attr.getMFVec3f(index, coords);
    for(std::vector<SFVec3f>::iterator I = coords.begin(); I != coords.end(); I++)
      {
      this->CurrentPoints->InsertNextPoint(&(*I).x);
      }
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startNormal(const X3DAttributes &attr)
  {
  this->CurrentNormals = vtkFloatArray::New();
  this->CurrentNormals->SetNumberOfComponents(3);

  if (checkReferencing(attr, &this->CurrentNormals, true))
    {
      if (!this->CurrentNormals)
        return SKIP_CHILDREN;

      this->CurrentNormals->Register(NULL);
      return CONTINUE;
    }

  int index = attr.getAttributeIndex(ID::vector);
  if(index != -1)
    {
    MFVec3f normals;
    attr.getMFVec3f(index, normals);
    for(std::vector<SFVec3f>::iterator I = normals.begin(); I != normals.end(); I++)
      {
      this->CurrentNormals->InsertNextTupleValue(&(*I).x);
      }
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startColor(const X3DAttributes &attr)
  {
  this->CurrentColors = vtkUnsignedCharArray::New();
  this->CurrentColors->SetNumberOfComponents(3);
  if (checkReferencing(attr, &this->CurrentColors, true))
    {
      if (!this->CurrentColors)
        return SKIP_CHILDREN;

      this->CurrentColors->Register(NULL);
      return CONTINUE;
    }

  int index = attr.getAttributeIndex(ID::color);
  if(index != -1)
    {
    MFColor colors;
    attr.getMFColor(index, colors);

    this->CurrentColors->SetNumberOfComponents(3);

    for(std::vector<SFColor>::iterator I = colors.begin(); I != colors.end(); I++)
      {
      this->CurrentColors->InsertNextTuple3((*I).r * 255, (*I).g * 255, (*I).b * 255);
      }
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startTextureCoordinate(const X3DAttributes &attr) 
  {
  this->CurrentTCoords = vtkFloatArray::New();
  this->CurrentTCoords->SetNumberOfComponents(2);
  if (checkReferencing(attr, &this->CurrentTCoords, true))
    {
      if (!this->CurrentTCoords)
        return SKIP_CHILDREN;

      this->CurrentTCoords->Register(NULL);
      return CONTINUE;
    }

  int index = attr.getAttributeIndex(ID::point);
  if(index != -1)
    {
    MFVec2f normals;
    attr.getMFVec2f(index, normals);

    for(std::vector<SFVec2f>::iterator I = normals.begin(); I != normals.end(); I++)
      {
      this->CurrentTCoords->InsertNextTuple2((*I).x, (*I).y);
      }
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startPointSet(const X3DAttributes &attr)
  {
  checkInShape(PointSet);
  
  vtkPolyDataMapper* pmap = vtkPolyDataMapper::New();
  if (checkReferencing(attr, &pmap, true))
    {
      if (!pmap)
        return SKIP_CHILDREN;
      this->CurrentActor->SetMapper(pmap);
    }
  else
    {
    pmap->SetScalarVisibility(0);
    // Points are alway unlit
    this->IsCurrentUnlit = 1;
    this->CurrentActor->SetMapper(pmap);
    pmap->Delete();
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::endPointSet() 
  {
  vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
  pd->SetVerts(cells);

  for (vtkIdType i=0;i < this->CurrentPoints->GetNumberOfPoints();i++) 
    {
    cells->InsertNextCell(1, &i);
    }

  vtkPolyDataMapper* pdm = vtkPolyDataMapper::SafeDownCast(this->CurrentActor->GetMapper());
  if (pdm)
    {
    pdm->SetInput(pd);
    pd->SetPoints(this->CurrentPoints);
    if(this->CurrentColors) 
      {
      pd->GetPointData()->SetScalars(this->CurrentColors);
      pdm->SetScalarVisibility(1);
      }
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startBox(const X3DAttributes &attr)
  {
  checkInShape(Box);

  vtkPolyDataMapper* pmap = vtkPolyDataMapper::New();
  if (checkReferencing(attr, &pmap, true))
    {
    this->CurrentActor->SetMapper(pmap);
    }
  else
    {
    // Check for size
    vtkSmartPointer<vtkCubeSource> cube= vtkSmartPointer<vtkCubeSource>::New();
    int index = attr.getAttributeIndex(ID::size);
    if(index != -1)
      {
      SFVec3f size;
      attr.getSFVec3f(index, size);

      cube->SetXLength(size.x);
      cube->SetYLength(size.y);
      cube->SetZLength(size.z);
      }
    else
      {
      cube->SetXLength(2.0f);
      cube->SetYLength(2.0f);
      cube->SetZLength(2.0f);
      }
    pmap->SetInput(cube->GetOutput());
    this->CurrentActor->SetMapper(pmap);
    pmap->Delete();
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startCone(const X3DAttributes &attr) 
  {
  checkInShape(Cone);
  
  vtkPolyDataMapper* pmap = vtkPolyDataMapper::New();
  if (checkReferencing(attr, &pmap, true))
    {
    this->CurrentActor->SetMapper(pmap);
    }
  else
    {
    vtkSmartPointer<vtkConeSource> cone= vtkSmartPointer<vtkConeSource>::New();
    // Check for height
    int index = attr.getAttributeIndex(ID::height);
    if(index != -1)
      {
      float height = attr.getSFFloat(index);
      cone->SetHeight(height);
      }
    else
      {
      cone->SetHeight(2.0f);
      }

    // Check for bottomRadius
    index = attr.getAttributeIndex(ID::bottomRadius);
    if(index != -1)
      {
      float bottomRadius = attr.getSFFloat(index);
      cone->SetRadius(bottomRadius);
      }
    else
      {
      cone->SetRadius(1.0f);
      }

    // Connect everything and delete
    pmap->SetInput(cone->GetOutput());
    this->CurrentActor->SetMapper(pmap);
    pmap->Delete();
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startCylinder(const X3DAttributes &attr) 
  {
  checkInShape(Cylinder);

  vtkPolyDataMapper* pmap = vtkPolyDataMapper::New();
  if (checkReferencing(attr, &pmap, true))
    {
    this->CurrentActor->SetMapper(pmap);
    }
  else
    {
    vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
    // Check for height
    int index = attr.getAttributeIndex(ID::height);
    if(index != -1)
      {
      float height = attr.getSFFloat(index);
      cylinder->SetHeight(height);
      }
    else
      {
      cylinder->SetHeight(2.0f);
      }

    // Check for radius
    index = attr.getAttributeIndex(ID::radius);
    if(index != -1)
      {
      float radius = attr.getSFFloat(index);
      cylinder->SetRadius(radius);
      }
    else
      {
      cylinder->SetRadius(1.0f);
      }

    pmap->SetInput(cylinder->GetOutput());
    this->CurrentActor->SetMapper(pmap);
    pmap->Delete();
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::startIndexedLineSet(const X3DAttributes &attr)
  {
  if (this->IsCurrentUSE)
    return CONTINUE;

  checkInShape(IndexedLineSet);

  vtkPolyDataMapper* pmap = vtkPolyDataMapper::New();
  if (checkReferencing(attr, &pmap, true))
    {
    this->CurrentActor->SetMapper(pmap);
    return CONTINUE;
    }
  this->CurrentActor->SetMapper(pmap);
  pmap->Delete();

  this->CurrentIndexedGeometry = vtkX3DIndexedGeometrySource::New();
  this->CurrentIndexedGeometry->SetGeometryFormatToIndexedLineSet();
  this->CurrentIndexedGeometry->SetDebug(this->Importer->GetDebug());

  this->IsCurrentUnlit = 1;

  // color per vertex
  int index = attr.getAttributeIndex(ID::colorPerVertex);
  if(index != -1 && !attr.getSFBool(index))
    {
    this->CurrentIndexedGeometry->ColorPerVertexOff();
    }
  else
    this->CurrentIndexedGeometry->ColorPerVertexOn();

  // coord index
  index = attr.getAttributeIndex(ID::coordIndex);
  if(index != -1)
    {
    std::vector<int> coords;
    attr.getMFInt32(index, coords);
    vtkIdTypeArray* idArray = vtkIdTypeArray::New();
    for(std::vector<int>::iterator I = coords.begin(); I != coords.end(); I++)
      {
      idArray->InsertNextValue(*I);
      }
    this->CurrentIndexedGeometry->SetCoordIndex(idArray);
    idArray->Delete();
    }

  // color index
  index = attr.getAttributeIndex(ID::colorIndex);
  if(index != -1)
    {
    std::vector<int> coords;
    attr.getMFInt32(index, coords);
    vtkIdTypeArray* idArray = vtkIdTypeArray::New();
    for(std::vector<int>::iterator I = coords.begin(); I != coords.end(); I++)
      {
      idArray->InsertNextValue(*I);
      }
    this->CurrentIndexedGeometry->SetColorIndex(idArray);
    idArray->Delete();
    }
  return CONTINUE;
  }

int vtkX3DNodeHandler::endIndexedLineSet()
  {
  vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();

  this->CurrentIndexedGeometry->SetCoords(this->CurrentPoints);
  this->CurrentIndexedGeometry->SetColors(this->CurrentColors);
  this->CurrentIndexedGeometry->Update();

 // This is to disconnect the geometry from the source  
  pd->ShallowCopy(this->CurrentIndexedGeometry->GetOutput());
  ((vtkPolyDataMapper*)this->CurrentActor->GetMapper())->SetInput(pd);

  vtkDebugWithObjectMacro(this->Importer, 
    << "Generated IndexedLineSet with " << pd->GetLines()->GetNumberOfCells() << " lines and "
    << pd->GetNumberOfPoints() << " points.");

  this->CurrentIndexedGeometry->Delete();
  this->CurrentIndexedGeometry = NULL;
  
  return CONTINUE;
  }

int vtkX3DNodeHandler::startDirectionalLight(const X3DAttributes &attr)
  {
  vtkLight* light = vtkLight::New();
  if (checkReferencing(attr, &light, true))
    {
    this->Renderer->AddLight(light);
    return CONTINUE;
    }
  this->Renderer->AddLight(light);
  light->Delete();

  // Check for ambientIntensity (SetIntensity does not really suit here)
  /*int index = attr.getAttributeIndex(ID::ambientIntensity);
  if(index != -1)
    {
    float ambientIntensity = attr.getSFFloat(index);
    light->SetIntensity(ambientIntensity);
    }
  else
    light->SetIntensity(1.0f);*/

  // Check for color
  int index = attr.getAttributeIndex(ID::color);
  if(index != -1)
    {
    SFColor color;
    attr.getSFColor(index, color);
    light->SetColor(color.r, color.g, color.b);
    }
  else
    light->SetColor(1.0f, 1.0f, 1.0f);

  // Check for direction
  index = attr.getAttributeIndex(ID::direction);
  if(index != -1)
    {
    SFVec3f direction;
    attr.getSFVec3f(index, direction);
    light->SetFocalPoint(direction.x, direction.y, direction.z);
    }
  else
    light->SetFocalPoint(0.0f, 0.0f, -1.0f);


  // Check for intensity
  index = attr.getAttributeIndex(ID::intensity);
  if(index != -1)
    {
    float intensity = attr.getSFFloat(index);
    light->SetIntensity(intensity);
    }
  else
    light->SetIntensity(1.0f);

  // Check for on
  index = attr.getAttributeIndex(ID::on);
  if(index != -1)
    {
    bool on = attr.getSFBool(index);
    light->SetSwitch(on ? 1 : 0);
    }
  else
    light->SetSwitch(1);

  return CONTINUE;
  }

int vtkX3DNodeHandler::startBackground(const X3DAttributes &attr)
  {
  if (this->SeenBindables & X3D_BACKGROUND)
    return CONTINUE;

  int index = attr.getAttributeIndex(ID::skyColor);
  if(index != -1)
    {
    MFColor skyColor;
    attr.getMFColor(index, skyColor);

    if (skyColor.size())
      {
      SFColor mainColor = skyColor[0];
      this->Renderer->SetBackground(mainColor.r, mainColor.g, mainColor.b);
      }
    }
  this->SeenBindables |= X3D_BACKGROUND;
  return CONTINUE;
  }

int vtkX3DNodeHandler::startNavigationInfo(const X3DAttributes &attr)
  {
  if (this->SeenBindables & X3D_NAVIGATIONINFO)
    return CONTINUE;
  
  int index = attr.getAttributeIndex(ID::headlight);
  if(index != -1 && !attr.getSFBool(index)) // Headlight off
    {
    this->Renderer->RemoveLight(this->HeadLight);
    }
  this->SeenBindables |= X3D_NAVIGATIONINFO;
  return CONTINUE;
  }

int vtkX3DNodeHandler::startViewpoint(const X3DAttributes &attr)
  {
  static const double up[3] = { 0.0, 1.0, 0.0 };
  static const double dir[3] = { 0.0, 0.0, -1.0 };

  if (this->SeenBindables & X3D_VIEWPOINT)
    return CONTINUE;
  
  vtkCamera* camera = this->Renderer->GetActiveCamera();
  
  // FOV
  int index = attr.getAttributeIndex(ID::fieldOfView);
  if (index != -1)
    {
    camera->SetViewAngle(vtkMath::DegreesFromRadians(attr.getSFFloat(index)));
    }
  else
    camera->SetViewAngle(45.0);

  double pos[3] = { 0.0, 0.0, 10.0 };
  index = attr.getAttributeIndex(ID::position);
  if(index != -1)
    {
    SFVec3f p;
    attr.getSFVec3f(index, p);
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
    }
  this->CurrentTransform->TransformPoint(pos, pos);
  camera->SetPosition(pos);

  index = attr.getAttributeIndex(ID::orientation);
  if (index != -1)
    {
    SFRotation ori;
    attr.getSFRotation(index, ori);
    vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
    t->DeepCopy(this->CurrentTransform);
    t->RotateWXYZ(vtkMath::DegreesFromRadians(ori.angle), ori.x, ori.y, ori.z);
    camera->SetViewUp(t->TransformDoubleNormal(up));
    double* dirVec = t->TransformDoubleNormal(dir);
    camera->SetFocalPoint(pos[0] + dirVec[0], pos[1] + dirVec[1], pos[2] + dirVec[2]);
    }
  else
    {
    camera->SetViewUp(up);
    camera->SetFocalPoint(pos[0], pos[1], pos[2] - 10.0);
    }

  this->SeenBindables |= X3D_VIEWPOINT;
  return CONTINUE;
  }

int  vtkX3DNodeHandler::startImageTexture(const XIOT::X3DAttributes &attr)
  {
  checkInShape(PixelTexture);

  vtkTexture* texture = vtkTexture::New();
  if (checkReferencing(attr, &texture, true))
    {
    this->CurrentActor->SetTexture(texture);
    return CONTINUE;
    }

  int index = attr.getAttributeIndex(ID::url);
  if (index != -1)
    {
    MFString url;
    attr.getMFString(index, url);
    if (url.size())
      {
      std::string fileName(this->Importer->GetFileName()), baseDir, fullName;
      size_t found = fileName.find_last_of("/\\");
      if (found != std::string::npos)
        baseDir = fileName.substr(0,found+1);

      fullName = baseDir + url[0];
      vtkDebugWithObjectMacro(this->Importer, << "Trying to load texture: " << fullName);
      vtkSmartPointer<vtkImageReader2> imageReader = vtkImageReader2Factory::CreateImageReader2(fullName.c_str());
      if (imageReader)
        {
        imageReader->SetFileName(fullName.c_str());
        imageReader->Update();
        texture->SetInputConnection(imageReader->GetOutputPort());
        texture->SetBlendingMode(vtkTexture::VTK_TEXTURE_BLENDING_MODE_NONE);
        texture->SetQualityTo32Bit();
        texture->RepeatOn();
        texture->InterpolateOn();

        this->CurrentActor->SetTexture(texture);
        }
      else
        {
        vtkWarningWithObjectMacro(this->Importer, << "Can not read texture: " << fullName);
        }
      }
    }
  texture->Delete();
  return CONTINUE;
  }

int  vtkX3DNodeHandler::startPixelTexture(const XIOT::X3DAttributes &attr)
  {
  checkInShape(PixelTexture);

  vtkTexture* texture = vtkTexture::New();
  if (checkReferencing(attr, &texture, true))
    {
    this->CurrentActor->SetTexture(texture);
    return CONTINUE;
    }

  int index = attr.getAttributeIndex(ID::image);
  if (index != -1)
    {
    SFImage image;
    attr.getSFImage(index, image);
    SFImage::const_iterator I = image.begin();
    unsigned int width = *(I++);
    unsigned int height = *(I++);
    unsigned int components = *(I++);
    vtkDebugWithObjectMacro(this->Importer, << "Creating Texture: width=" << width << ", height="<< height<<", components="<<components);
    if (static_cast<unsigned int>(image.size()) != width*height+3)
      {
      vtkErrorWithObjectMacro(this->Importer, << "PixelTexture has wrong size. Expected="<< width*height << ",  Found="<< image.size()-3);
      }
    else
      {
      vtkSmartPointer<vtkUnsignedCharArray> scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
      scalars->SetNumberOfComponents(components);
      scalars->SetNumberOfValues(width*height*components);
      vtkIdType i = 0;
      while(I != image.end())
        {
        union conv
          {
          unsigned int ui;
          unsigned char ub[4]; 
          };
        conv c;
        c.ui = *I;
        switch(components)
          {
          case 4:
            scalars->SetValue(i++, c.ub[3]);
          case 3:
            scalars->SetValue(i++, c.ub[2]);
          case 2:
            scalars->SetValue(i++, c.ub[1]);
          case 1:
            scalars->SetValue(i++, c.ub[0]);
            break;
          default:
            vtkErrorWithObjectMacro(this->Importer, << "Unsupported component size: " << components);
            break;

          };
        I++;
        }

      vtkSmartPointer<vtkImageData> id = vtkSmartPointer<vtkImageData>::New();
      id->SetDimensions(width, height, 1); // 1 is for 2D Texture
      id->SetScalarTypeToUnsignedChar();
      id->SetNumberOfScalarComponents(components);
      id->GetPointData()->SetScalars(scalars);
      texture->SetInput(id);
      texture->SetQualityTo32Bit();
      texture->RepeatOn();
      texture->InterpolateOn();
      texture->SetBlendingMode(vtkTexture::VTK_TEXTURE_BLENDING_MODE_REPLACE);
      this->CurrentActor->SetTexture(texture);
      }
    }
  texture->Delete();
  return CONTINUE;
  }

int vtkX3DNodeHandler::startUnhandled(const char* nodeName, const X3DAttributes &vtkNotUsed(attr))
  {
  int elementID = X3DTypes::getElementID(nodeName);

  if (_ignoreNodes.find(elementID) == _ignoreNodes.end())
    {
    vtkDebugWithObjectMacro(this->Importer, << "Ignoring node " << nodeName << " and all of it's children.");
    return SKIP_CHILDREN;
    }
  vtkDebugWithObjectMacro(this->Importer, << "Ignoring node " << nodeName << ".");
  return CONTINUE;
  }





//-----------------------------------------------------------------------------
// vtkX3DImporter
//-----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkX3DImporter, "$Revision: 1.19 $");
vtkStandardNewMacro(vtkX3DImporter);

vtkX3DImporter::vtkX3DImporter()
{
	this->FileName = NULL;
  this->CalculateNormals = 1;
}

vtkX3DImporter::~vtkX3DImporter()
{
	if (this->FileName)
    {
    delete [] this->FileName;
    }
}

int vtkX3DImporter::ImportBegin()
{
	XIOT::X3DLoader loader;
	XIOT::X3DTypes::initMaps();
  vtkX3DNodeHandler handler(this->Renderer, this);
	loader.setNodeHandler(&handler);
	try {
		if (!loader.load(this->FileName))
			return 0;
	} 
	catch (XIOT::X3DParseException& e)
	{	
	  vtkErrorMacro( << "Error while parsing file " << this->FileName << ":" << e.getMessage().c_str()
					  << " (Line: " << e.getLineNumber() << ", Column: " << e.getColumnNumber() << ")");
      return 0;
	}
	return 1;
}

void vtkX3DImporter::ImportEnd ()
{
}

class ShapeCounter : public XIOT::X3DDefaultNodeHandler {
  public:
    virtual void startDocument() 
    {
      _count = 0;
    }

    virtual int startShape(const XIOT::X3DAttributes &)
    {
      _count++;
	    return XIOT::CONTINUE;
    }

    int _count;  
};

int vtkX3DImporter::GetNumberOfShapes(char* FileName)
{
  XIOT::X3DLoader loader;
	ShapeCounter handler;
	loader.setNodeHandler(&handler);
	try {
    loader.load(FileName);
	} catch (XIOT::X3DParseException&)
	{	
      return -1;
	}
	return handler._count;
}


void vtkX3DImporter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "File Name: " 
     << (this->FileName ? this->FileName : "(none)") << "\n";
  os << indent << "CalculateNormals: " 
     << this->CalculateNormals << "\n";
}


