#ifndef X3D_X3DXMLLOADER_H
#define X3D_X3DXMLLOADER_H

#include "X3DLoader.h"
#include <string>

namespace XIOT {

/**
 * @class XMLParserImpl
 * Wrapper to hide the XercesC Implementation from the interface.
 */
class XMLParserImpl;

/**  
 *  Loader for XML encoded X3D files. 
 *
 *  The class uses an XercesC XML parser internally to generate the events for the X3DNodeHandler.
 *  Instead of using this class directly you can use the X3DLoader which will delgate to the
 *  right encoding implementation depending on the files suffix.
 *  
 *  @see X3DLoader
 *  @ingroup x3dloader
 */
class X3DLOADER_EXPORT X3DXMLLoader : public X3DLoader
{
public:
  /// Constructor.
  X3DXMLLoader();
  /// Destructor.
  virtual ~X3DXMLLoader();
  
  /** Loads an X3D scene graph from the file. If fileValidation is true, then the file will be 
  * verified. 
  * @return True, if loading was successful.
  * @exception X3DParseException If a parsing error occures that cannot be handled.
  */
  bool load(std::string fileName, bool fileValidation = true) const;
  
protected:
	XMLParserImpl *_impl;
};

}

#endif
