#include "OgreNodeHandler.h"
#include "X3DAttributes.h"
#include "Ogre.h"
#include "GeomUtils.h"
#include "IndexedGeometry.h"
#include <cassert>

using namespace Ogre;
using namespace std;

OgreNodeHandler::OgreNodeHandler()
{
	_sceneManager = NULL;
	_currentMaterial.setNull();
	_currentEntity = NULL;
	_currentManualObject = NULL;
	_objCount = 0;
	_currentGeometry = NULL;
}

OgreNodeHandler::~OgreNodeHandler()
{
}

void OgreNodeHandler::setSceneManager(Ogre::SceneManager* sceneManager)
{
	_sceneManager = sceneManager;
}


void OgreNodeHandler::startDocument()
{
	assert(_sceneManager);
	_nodeStack.push(_sceneManager->getRootSceneNode());
}

void OgreNodeHandler::endDocument()
{
}

int OgreNodeHandler::startShape(const X3D::X3DAttributes &attr) {
  std::cout << "Start Shape" << std::endl;
  if (attr.isUSE()) {
	  _currentEntity = _sceneManager->getEntity(attr.getUSE())->clone(createUniqueName(attr, "shapeUSE"));
  } else  {
	  // We can not create a entity yet, because Ogre does not
	  // allow an entity without a mesh. So we create the entity as
	  // soon as we have a mesh
	  _shapeName = attr.isDEF() ? attr.getDEF() : createUniqueName(attr, "shape");
  }

  return 1;
}

int OgreNodeHandler::endShape() {
  std::cout << "End Shape\n";

  if (!_currentEntity && !_currentManualObject)
  {
		std::cout << "No geometry found\n";
		return 1;
  }
  if (_currentEntity)
  {
	  _nodeStack.top()->attachObject(_currentEntity);
	  if (_currentMaterial.isNull())
	  {
		  _currentEntity->getSubEntity(0)->setMaterialName("_X3DNOMATERIAL");
		  //_currentMaterial = MaterialManager::getSingleton().create("_x3ddefaultmaterial", "X3DRENDERER");
	  }
	  else
	  {
		//std::cout << "Setting active Material: " <<  static_cast<Ogre::Entity*>(obj)->getSubEntity(0)->getMaterialName() << std::endl;
		_currentEntity->getSubEntity(0)->setMaterialName(_currentMaterial->getName());
		Ogre::MaterialSerializer serial;
		serial.queueForExport(_currentMaterial);
		std::cout << "Current material:" << std::endl << serial.getQueuedAsString();
		serial.clearQueue();
		std::cout << "Material source blend factor: " << _currentMaterial->getTechnique(0)->getPass(0)->getSourceBlendFactor();
		std::cout << "Material destination blend factor: " << _currentMaterial->getTechnique(0)->getPass(0)->getDestBlendFactor();
		std::cout << (_currentMaterial->isTransparent() ? "Material is transparent" : "Material is opaque");	
	  }
	  std::cout << "Setting active Material: " << _currentEntity->getSubEntity(0)->getMaterialName() << std::endl;
	  _currentEntity = NULL;
  }
  if (_currentManualObject)
  {
	_nodeStack.top()->attachObject(_currentManualObject);
	_currentManualObject = NULL;
  }

  _currentMaterial.setNull();
  _shapeName.clear();
  return 1;
}

int OgreNodeHandler::startTransform(const X3D::X3DAttributes &attr) {
  std::cout << "Start Transform" << std::endl;
  // Get all the X3D tranformation data
  X3D::SFVec3f translation;
  X3D::SFRotation rotation;

  int index = attr.getAttributeIndex(X3D::translation);
  if (index != -1)
  {
	  translation = attr.getSFVec3f(index);
  }
  index = attr.getAttributeIndex(X3D::rotation);
  if (index != -1)
  {
	  rotation = attr.getSFRotation(index);
  }

  SceneNode* node = _nodeStack.top()->createChildSceneNode(createUniqueName(attr, "transform"));
  node->resetToInitialState();
  node->translate(translation.x, translation.y, translation.z);
  node->rotate(Vector3(rotation.x, rotation.y, rotation.z), Radian(rotation.angle));
  _nodeStack.push(node);
  return 1;
}

int OgreNodeHandler::endTransform() {
  std::cout << "End Transform" << std::endl;
  _nodeStack.pop();
  return 1;
}


int OgreNodeHandler::startSphere(const X3D::X3DAttributes &attr) {
  std::cout << "Start Sphere\n";

  int index = attr.getAttributeIndex(X3D::radius);
  float radius = index == -1 ? 1.0 : attr.getSFFloat(index);

  const string name = createUniqueName(attr, "sphere");
  GeomUtils::createSphere(name , radius, 20, 20, true, false);

  _currentMesh = MeshManager::getSingleton().getByName(name);
  _currentEntity = _sceneManager->createEntity(_shapeName, name);
  return 1;
}

int OgreNodeHandler::startBox(const X3D::X3DAttributes &attr) {
  std::cout << "Start Box\n";
  _currentEntity = _sceneManager->createEntity(createUniqueName(attr, "shape"), "cube.mesh");

  return 1;
}




int OgreNodeHandler::startAppearance(const X3D::X3DAttributes &attr) {
  std::cout << "Start Appearance" << std::endl;
  _currentMaterial = MaterialManager::getSingleton().create(createUniqueName(attr, "material"), "X3DRENDERER");
  _currentMaterial->load();
  _currentMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
  return 1;
}

int OgreNodeHandler::endAppearance() {
  std::cout << "End Appearance" << std::endl;
  return 1;
}

int OgreNodeHandler::startMaterial(const X3D::X3DAttributes &attr) {
  std::cout << "Start Material" << std::endl; 
  if (!_currentMaterial.isNull())
  {
	  Pass* pass = _currentMaterial->getTechnique(0)->getPass(0);
	  int index = attr.getAttributeIndex(X3D::ambientIntensity);
	  float ambientIntensity = (index == -1) ? 0.2f : attr.getSFFloat(index);
	  index = attr.getAttributeIndex(X3D::transparency);
	  float transparency = (index == -1) ? 0.0f : attr.getSFFloat(index);
	  
	  X3D::SFColor diffuseColor;
	  index = attr.getAttributeIndex(X3D::diffuseColor);
	  if (index != -1)
	  {
		  diffuseColor = attr.getSFColor(index);
	  }
	  else 
	  {
		  diffuseColor.r = diffuseColor.g = diffuseColor.b = 0.8;
	  }

	  X3D::SFColor specularColor;
	  index = attr.getAttributeIndex(X3D::specularColor);
	  if (index != -1)
	  {
		  specularColor = attr.getSFColor(index);
	  }

  	  X3D::SFColor emissiveColor;    
	  index = attr.getAttributeIndex(X3D::emissiveColor);
	  if (index != -1)
	  {
		  emissiveColor = attr.getSFColor(index);
	  }
	  
	  index = attr.getAttributeIndex(X3D::shininess);
	  float shininess = (index == -1) ? 0.2f : attr.getSFFloat(index);
	  shininess = Math::Clamp(shininess * 128.0f, 0.0f, 128.0f);

	  pass->setAmbient(ambientIntensity * diffuseColor.r,
					   ambientIntensity * diffuseColor.g,
					   ambientIntensity * diffuseColor.b);
	  pass->setDiffuse(diffuseColor.r,
					   diffuseColor.g,
					   diffuseColor.b,
					   1.0f - transparency);
	  pass->setSpecular(specularColor.r,
					   specularColor.g,
					   specularColor.b,
					   1.0f - transparency);
	  
	  pass->setSelfIllumination(emissiveColor.r,
								emissiveColor.g,
								emissiveColor.b);

	  pass->setShininess( shininess );
      pass->setLightingEnabled(true);
  }

  return 1;
}

int OgreNodeHandler::startCoordinate(const X3D::X3DAttributes &attr)
{
	if(!_currentGeometry)
		throw std::runtime_error("Coordinates currently only supported for IndexedFaceSets");
	int index = attr.getAttributeIndex(X3D::point);
	if (index != -1)
		_currentGeometry->setCoords(attr.getMFVec3f(index));
	else
		throw std::runtime_error("No points given within Coordinate node");
	return 1;
}

int OgreNodeHandler::startNormal(const X3D::X3DAttributes &attr)
{
	if(!_currentGeometry)
		throw std::runtime_error("Normal currently only supported for IndexedFaceSets");
	int index = attr.getAttributeIndex(X3D::vector);
	if (index != -1)
		_currentGeometry->setNormals(attr.getMFVec3f(index));
	else
		throw std::runtime_error("No points given within Coordinate node");
	return 1;
}

int OgreNodeHandler::startColor(const X3D::X3DAttributes &attr)
{
	if(!_currentGeometry)
		throw std::runtime_error("Color currently only supported for IndexedFaceSets");
	int index = attr.getAttributeIndex(X3D::color);
	if (index != -1)
		_currentGeometry->setColors(attr.getMFColor(index));
	else
		throw std::runtime_error("No color given within Color node");
	return 1;
}

int OgreNodeHandler::startIndexedLineSet(const X3D::X3DAttributes &attr) {
  
  _currentGeometry = new IndexedGeometry(createUniqueName(attr, "indexedLineSet"));

  int index = attr.getAttributeIndex(X3D::coordIndex);
  if (index != -1)
	  _currentGeometry->setCoordIndex(attr.getMFInt32(index));

  index = attr.getAttributeIndex(X3D::colorPerVertex);
  _currentGeometry->setColorPerVertex(index != -1 ? attr.getSFBool(index) : true);

  return X3D::CONTINUE;
}

int OgreNodeHandler::endIndexedLineSet() {
  std::cout << "End IndexedLineSet" << std::endl;
 
  _currentManualObject = _sceneManager->createManualObject(_shapeName);
  _currentGeometry->createIndexedLineSet(_currentManualObject);
  delete _currentGeometry;
  _currentGeometry = NULL;

  return X3D::CONTINUE;
}

int OgreNodeHandler::startIndexedFaceSet(const X3D::X3DAttributes &attr) {
	std::cout << "Start IndexedFaceSet" << std::endl;
  
  _currentGeometry = new IndexedGeometry(createUniqueName(attr, "indexedFaceSet"));

  int index = attr.getAttributeIndex(X3D::coordIndex);
  if (index != -1)
	  _currentGeometry->setCoordIndex(attr.getMFInt32(index));
  
  index = attr.getAttributeIndex(X3D::normalIndex);
  if (index != -1)
	  _currentGeometry->setNormalIndex(attr.getMFInt32(index));

  index = attr.getAttributeIndex(X3D::normalPerVertex);
  _currentGeometry->setNormalPerVertex(index != -1 ? attr.getSFBool(index) : true);


  return 1;
}

int OgreNodeHandler::endIndexedFaceSet() {
  std::cout << "End IndexedFaceSet" << std::endl;
  _currentGeometry->createIndexedFaceSet();
  _currentEntity = _sceneManager->createEntity(_shapeName, _currentGeometry->getName());
  delete _currentGeometry;
  _currentGeometry = NULL;
  return 1;
}

int OgreNodeHandler::startDirectionalLight(const X3D::X3DAttributes &attr) {
	Light* light = _sceneManager->createLight(createUniqueName(attr, "directionalLight"));
	light->setType(Ogre::Light::LT_DIRECTIONAL);

	// Get the X3D values;
	X3D::SFVec3f direction;
	X3D::SFColor color;

	int index = attr.getAttributeIndex(X3D::direction);
	if (index != -1)
	{
		direction = attr.getSFVec3f(index);
	}
	else
		direction.z = -1;

	index = attr.getAttributeIndex(X3D::color);
	if (index != -1)
	{
		color = attr.getSFColor(index);
	}
	else
		color.r = color.g = color.b = 1;

	index = attr.getAttributeIndex(X3D::intensity);
	float intensity = (index == -1) ? 1 : attr.getSFFloat(index);

	index = attr.getAttributeIndex(X3D::on);
	bool on = (index == -1) ? true : attr.getSFBool(index);

	// Set the Ogre values
	light->setDirection(direction.x, direction.y, direction.z);

	Ogre::ColourValue colourValue(color.r,  color.g, color.b);
	colourValue *= intensity;

	light->setDiffuseColour(colourValue);
	light->setSpecularColour(colourValue);
	light->setVisible(on);
	_nodeStack.top()->attachObject(light);  
	return 1;
}


int OgreNodeHandler::startUnknown(const char* nodeName, const X3D::X3DAttributes &attr) {
   std::cout << "Found unknown node" << std::endl;
  return 1;
}


std::string OgreNodeHandler::createUniqueName(const X3D::X3DAttributes &attr, const std::string& prefix)
{
	if (attr.isDEF())
		return attr.getDEF();

	string result;
	result.append(prefix);
	result.append(StringConverter::toString(++_objCount));
	return result;
}