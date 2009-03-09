#include "TutorialApplication.h"
#include "X3DLoader.h"
#include "OgreNodeHandler.h"


void TutorialApplication::setX3DFile(const std::string& filename)
{
	_filename = filename;
}

void TutorialApplication::createScene()
{
	// New instance of X3DLoader
	X3D::X3DLoader loader;
	// Create and set NodeHandler
	OgreNodeHandler* handler = new OgreNodeHandler();
	loader.setNodeHandler(handler);
	// Start processing
	loader.load(_filename);
}

