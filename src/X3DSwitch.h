#ifndef X3D_X3DSWITCH_H
#define X3D_X3DSWITCH_H

#include <string>

namespace XIOT {

// forward declarations
class X3DAttributes;
class X3DNodeHandler;

/** 
 * The <b>X3DSwitch</b> for the handling of different nodes.
 * It supports the call {@link #doStartElement(int id, const X3DAttributes& attr)}
 * to invoke the <code>startXXX</code> method of the X3DNodeHandler
 * for each node of the X3D spec, resp. the call 
 * {@link #doEndElement(int id, const char* nodeName = 0)}
 * to invoke the <code>endXXX</code> method of the X3DNodeHandler.
 *
 * @see X3DNodeHandler
 */

class X3DSwitch
{
public:
  /// Constructor.
  X3DSwitch();
  /// Destructor.
  virtual ~X3DSwitch();
  
  /**
   * Calls <code>startXXX</code> of the given X3DNodeHandler for each node
   * of the X3D model.
   *
   * @param id The id of the node as specified in 
   * @link{http://www.web3d.org/x3d/specifications/ISO-IEC-FCD-19776-3.2-X3DEncodings-CompressedBinary/Part03/tables.html} 
   * or <b>-1</b> if the node id is unknown. In this case, the name of the current node may be accessible
   * through X3DAttributes::getNodeName().
   * @parm attr Class that can be accessed to receive the nodes attributes
   */
  int doStartElement(int id, const X3DAttributes& attr) const;
  
  /**
   * Calls <code>endXXX</code> of the given X3DNodeHandler for each node
   * of the X3D model.
   *
   * @param id The id of the node as specified in 
   * @link{http://www.web3d.org/x3d/specifications/ISO-IEC-FCD-19776-3.2-X3DEncodings-CompressedBinary/Part03/tables.html} 
   * or <b>-1</b> if the node id is unknown. In this case, the name of the current node may be accessible
   * through nodeName
   * @param nodeName Optionally parameter that holds the name of the current node.
   * @parm attr Class that can be accessed to receive the nodes attributes
   */
  int doEndElement(int id, const char* nodeName = 0) const;

  /**
   * Set the X3DNodeHandler that the start and end callbacks will be delegated to.
   * @param handler The handler. Must not be <code>null</code>.
   */
  void setNodeHandler(X3DNodeHandler *handler);
  
protected:
  /// Handler.
  X3DNodeHandler *_handler;
};

}

#endif
