#ifndef X3D_X3DFILOADER_H
#define X3D_X3DFILOADER_H

#include "X3DLoader.h"
#include "X3DNodeHandler.h"
#include "X3DSwitch.h"
#include "X3DFIAttributes.h"
#include "FITypes.h"
#include <string>
#include <fstream>

namespace XIOT {

/**  
 *  Loader for FI (binary) encoded X3D files. 
 *
 *  The class uses the openFI parser implementation to generate the events for the X3DNodeHandler.
 *  Instead of using this class directly you can use the X3DLoader which will delgate to the
 *  right encoding implementation depending on the files suffix.
 *  
 *  @see X3DLoader
 */
class X3DLOADER_EXPORT X3DFILoader : public X3DLoader
{
public:
  /// Constructor.
  X3DFILoader();
  /// Destructor.
  virtual ~X3DFILoader();
 
  /**
   * @see X3DLoader::load()
   */ 
  bool load(std::string fileName, bool fileValidation = true); // C.2

  
protected:
	
private:
	std::ifstream file;
	X3DNodeHandler *handler;
	X3DSwitch	x3dswitch;
};

}

#endif
