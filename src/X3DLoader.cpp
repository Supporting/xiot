#include "X3DLoader.h"
#include "X3DXMLLoader.h"
#include "X3DFILoader.h"
#include "X3DParseException.h"
#include "X3DTypes.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace X3D {

X3DLoader::X3DLoader()
: _handler(NULL)
{
}

X3DLoader::~X3DLoader()
{
}

bool X3DLoader::load(std::string fileName, bool fileValidation) const
{
	assert(_handler);
	X3DTypes::initMaps();
	std::string extensionStr = fileName.substr(fileName.find_last_of('.') + 1, fileName.size());
	if (extensionStr == "x3d")
	{
		X3DXMLLoader xmlLoader;
		xmlLoader.setNodeHandler(_handler);
		return xmlLoader.load(fileName, fileValidation);
	}
	else if  (extensionStr == "x3db")
	{
		X3DFILoader fiLoader;
		fiLoader.setNodeHandler(_handler);
		return fiLoader.load(fileName, fileValidation);
	}
	return false;
}

void X3DLoader::setNodeHandler(X3DNodeHandler *handler)
{
	this->_handler = handler;
}


}
