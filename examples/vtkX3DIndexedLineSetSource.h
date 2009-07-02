#ifndef __vtkX3DIndexedLineSetSource_h
#define __vtkX3DIndexedLineSetSource_h

#include <vtkPolyDataAlgorithm.h>

class vtkIdTypeArray;
class vtkFloatArray;
class vtkUnsignedCharArray;

/**
 * Generate vtkPolyData from X3DIndexedLineSet
 *
 * Helper class to generate a vtkPolyData structure from 
 * an IndexedLineSet data structure. This class converts index
 * lists and resolves multiple used coordinate data.
 * @see <A href="http://www.vtk.org/doc/nightly/html/classvtkPolyDataAlgorithm.html"/>vtkPolyDataAlgorithm</A>
 * @see <A href="http://www.web3d.org/x3d/specifications/ISO-IEC-FDIS-19775-1.2-X3D-AbstractSpecification/Part01/components/geometry3D.html#IndexedFaceSet"/>IndexedFaceSet</A>
 * @ingroup vtkX3DImporter
 */
class vtkX3DIndexedLineSetSource : public vtkPolyDataAlgorithm
{
public:
  static vtkX3DIndexedLineSetSource *New();
  vtkTypeRevisionMacro(vtkX3DIndexedLineSetSource,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(ColorPerVertex ,int);
  vtkGetMacro(ColorPerVertex,int);
  vtkBooleanMacro(ColorPerVertex,int);

  // Indices

  // Description:
  // Set the coord index array defining vertices.
  // This is in X3D format with -1 splitting cells
  void SetCoordIndex(vtkIdTypeArray * v);
  vtkIdTypeArray *GetCoordIndex() { return this->CoordIndex; };
  
  void SetColorIndex(vtkIdTypeArray * v);
  vtkIdTypeArray *GetColorIndex() { return this->ColorIndex; };


  // Data
  void SetCoords(vtkPoints *p);
  vtkPoints* GetCoords() { return this->Coords; };

  void SetColors(vtkUnsignedCharArray* p);
  vtkUnsignedCharArray* GetColors(){ return this->Colors; };
  

protected:
  vtkX3DIndexedLineSetSource();
  ~vtkX3DIndexedLineSetSource();

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int ColorPerVertex;

  vtkIdTypeArray* CoordIndex;
  vtkIdTypeArray* ColorIndex;

  vtkPoints *Coords;
  vtkUnsignedCharArray* Colors;


private:


  vtkX3DIndexedLineSetSource(const vtkX3DIndexedLineSetSource&); // Not implemented.
  void operator=(const vtkX3DIndexedLineSetSource&); // Not implemented.
};


#endif
