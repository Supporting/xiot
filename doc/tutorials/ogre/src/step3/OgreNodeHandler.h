#ifndef X3D_X3DLogNodeHandler_H
#define X3D_X3DLogNodeHandler_H

#include "X3DDefaultNodeHandler.h"
#include "OgreMaterial.h"
#include "OgreMesh.h"
#include <stack>

namespace Ogre {
	class SceneManager;
	class SceneNode;
};

class IndexedFaceSet;

/**
 * Handler that logs all library events to a file.
 *
 * This is yet another class that derives from the X3DDefaultNodeHandler. 
 * It overrides each callback function to create a log file,
 * which reports every start and end of a XML node/event. 
 * Plus, it will count the number of  events - thus nodes - 
 * occuring throughout the file import.
 * @see X3DDefaultNodeHandler
 */
class OgreNodeHandler : public X3D::X3DDefaultNodeHandler {

public:

  OgreNodeHandler();
  ~OgreNodeHandler();

  void setSceneManager(Ogre::SceneManager* sceneMgr);

  void startDocument();
  void endDocument();

  int startShape(const X3D::X3DAttributes &attr);
  int endShape();

  int startSphere(const X3D::X3DAttributes &attr);

  int startBox(const X3D::X3DAttributes &attr);

  int startAppearance(const X3D::X3DAttributes &attr);
  int endAppearance();

  int startMaterial(const X3D::X3DAttributes &attr);

  int startTransform(const X3D::X3DAttributes &attr);
  int endTransform();

  int startDirectionalLight(const X3D::X3DAttributes &attr);

  int startUnknown(const char* nodeName, const X3D::X3DAttributes &attr);
  int endUnknown(int id, const char* nodeName);

  protected:
	  std::string createUniqueName(const X3D::X3DAttributes &attr, const std::string& prefix="");

  private:
	  Ogre::SceneManager* _sceneManager;
	  std::stack<Ogre::SceneNode*> _nodeStack;
	  Ogre::MaterialPtr _currentMaterial;
	  Ogre::Entity* _currentEntity;
	  Ogre::MeshPtr _currentMesh;
	  IndexedFaceSet* _currentIFS;
	  std::string _shapeName;
	  int _objCount;
};

#endif

