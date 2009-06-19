#ifndef X3D_X3DFIATTRIBUTES_H
#define X3D_X3DFIATTRIBUTES_H

#include "X3DAttributes.h"
#include "FITypes.h"

namespace FI {
	class ParserVocabulary;
};

namespace XIOT {

class FIAttributeImpl;

/**
 * Stores the attributes of an Fi encoded XML element
 *
 * This class is the FI implementation of X3DAttributes and 
 * provides functions to retrieve the value of an attribute,
 * specified by its index. 
 *
 * It's also able to retrieve the index of an element
 * by its ID, as specified in:
 * @link(http://www.web3d.org/x3d/specifications/ISO-IEC-FCD-19776-3.2-X3DEncodings-CompressedBinary/Part03/tables.html)
 *
 * Resolving the binary ocetet attribute values to the X3D types is
 * done directly if possible, i.e. by using the encoding algorithms.
 * Otherwise it is delegated to the ParserVocabulary.
 *
 * @see X3DAttributes
 * @see X3DParserVocabulary
 * @ingroup x3dloader
 */
class X3DLOADER_EXPORT X3DFIAttributes : public X3DAttributes
{
public:
  /// Constructor.
  X3DFIAttributes(const void *const attributes, const FI::ParserVocabulary* vocab);

  /// Destructor.
  virtual ~X3DFIAttributes();

  void addAttribute(FI::Attribute &attr);

  virtual int getAttributeIndex(int attributeID) const;
  virtual size_t getLength() const;
  virtual std::string getAttributesAsString() const;

  // Single fields
  virtual bool getSFBool(int index) const;
  virtual float getSFFloat(int index) const;
  virtual int getSFInt32(int index) const;

  virtual SFVec3f getSFVec3f(int index) const;
  virtual SFVec2f getSFVec2f(int index) const;
  virtual SFRotation getSFRotation(int index) const;
  virtual std::string getSFString(int index) const;
  virtual SFColor getSFColor(int index) const;
  virtual SFColorRGBA getSFColorRGBA(int index) const;
  virtual SFImage getSFImage(int index) const; 

  // Multi Field
  virtual std::vector<float> getMFFloat(int index) const;
  virtual std::vector<int> getMFInt32(int index) const;

  virtual std::vector<SFVec3f> getMFVec3f(int index) const;
  virtual std::vector<SFVec2f> getMFVec2f(int index) const;
  virtual std::vector<SFRotation> getMFRotation(int index) const;
  virtual std::vector<std::string> getMFString(int index) const;
  virtual std::vector<SFColor> getMFColor(int index) const;
  virtual std::vector<SFColorRGBA> getMFColorRGBA(int index) const;

protected:
  std::vector<float> getFloatArray(const FI::NonIdentifyingStringOrIndex &value) const;
  std::vector<int>   getIntArray(const FI::NonIdentifyingStringOrIndex &value) const;

  FIAttributeImpl* _impl;
};

}; // End namespace X3D;


#endif

