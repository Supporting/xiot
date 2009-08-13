#ifndef __vtkX3DNodeHandler_h
#define __vtkX3DNodeHandler_h

#include <map>
#include <set>

#include <xiot/X3DDefaultNodeHandler.h>
#include "vtkRenderer.h"

class vtkActor;
class vtkAlgorithm;
class vtkProperty;
class vtkCamera;
class vtkLight;
class vtkTransform;
class vtkLookupTable;
class vtkFloatArray;
class vtkPolyDataMapper;
class vtkPoints;
class vtkIdTypeArray;
class vtkVRMLImporterInternal;
class vtkCellArray;
class vtkX3DIndexedGeometrySource;
class vtkUnsignedCharArray;

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
	
	vtkX3DNodeHandler(vtkRenderer* Renderer);
	~vtkX3DNodeHandler();

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
	
	int startBackground(const XIOT::X3DAttributes &attr);

	void setVerbose(bool verbose);
private:
	vtkRenderer			*Renderer;

	vtkActor			*actor;
	vtkPolyDataMapper	*pmap;

	vtkActor             *CurrentActor;
	vtkProperty          *CurrentProperty;
	vtkCamera            *CurrentCamera;
	vtkLight             *CurrentLight;
	vtkTransform         *CurrentTransform;
	vtkAlgorithm         *CurrentSource;
	vtkPoints            *CurrentPoints;
	vtkFloatArray        *CurrentNormals;
	vtkFloatArray        *CurrentTCoords;
	vtkCellArray         *CurrentTCoordCells;
	vtkLookupTable       *CurrentLut;
	vtkFloatArray        *CurrentScalars;
	vtkPolyDataMapper    *CurrentMapper;
	vtkUnsignedCharArray *CurrentColors;
	vtkX3DIndexedGeometrySource *CurrentIndexedGeometry;

	std::map<std::string,vtkActor*>			defShape;		// used for DEF/USE
	std::map<std::string,vtkUnsignedCharArray*>		defColor;	// used for DEF/USE
	std::map<std::string,vtkPoints*>		defCoordinate;	// used for DEF/USE
	std::map<std::string,vtkFloatArray*>	defTextureCoordinates;	// used for DEF/USE
	std::map<std::string,vtkFloatArray*>	defNormals;	// used for DEF/USE

	std::set<int>						_ignoreNodes;
	std::string								defString; // current DEF (used for endShape())
	
	bool	bDEFCoordinate;
	bool	bDEFTextureCoordinate;
	bool	bDEFColor;
	bool	bDEFNormal;
	bool	_verbose;


};

#endif

