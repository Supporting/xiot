#ifndef X3D_X3DPARSEEXCEPTION_H
#define X3D_X3DPARSEEXCEPTION_H

#include "X3DLoaderConfigure.h"
#include <string>

namespace XIOT {

/** 
 * An X3D parse error or warning.
 *
 * <p>This exception will include information for locating the error
 * in the original X3D document.
 * @ingroup x3dloader
 */
class X3DLOADER_EXPORT X3DParseException
{
public:
  /// Constructor.
	X3DParseException(const std::string &message, int lineNumber, int columnNumber);
  /// Constructor.
	X3DParseException(const std::string &message);

  
  /**
   *  Get the reason for the parse exception 
   */
  virtual const std::string getMessage() const { return _message; }
  
  /**
  * The line number of the end of the text where the exception occurred.
  *
  * @return An integer representing the line number, or 0
  *         if none is available.
  */
  virtual int getLineNumber() const { return _lineNumber; }

  /**
  * The column number of the end of the text where the exception occurred.
  *
  * <p>The first column in a line is position 1.</p>
  *
  * @return An integer representing the column number, or 0
  *         if none is available.
  */
  virtual int getColumnNumber() const { return _columnNumber; }

protected:
	int _columnNumber;
	int _lineNumber;
	std::string _message;
};

} // namespace X3D

#endif

