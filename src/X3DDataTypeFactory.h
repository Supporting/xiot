#ifndef X3D_X3DDATATYPEFACTORY_H
#define X3D_X3DDATATYPEFACTORY_H

#include <string>

#include "X3DTypes.h"

namespace XIOT {

/**
 * The <b>X3DDataTypeFactory</b> provides getXXXFromString functions which parse a given 
 * string and return the corresponding value (as specified by XXX, e.g. getSFBoolFromString). 
 * These functions are utilized by the X3DXMLAttributes class.
 *
 * @see X3DXMLAttributes
 * @ingroup x3dloader
 */
class X3DLOADER_EXPORT X3DDataTypeFactory
{
public:

  /**
   * Parses a given string and returns its value as a bool.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static bool getSFBoolFromString(const std::string &s);

  /**
   * Parses a given string and returns its value as a bool.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  //static bool getSFBoolFromBytes(const std::string &s);

  /**
   * Parses a given string and returns its value as a float.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static float getSFFloatFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as an integer.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static int getSFInt32FromString(const std::string &s);

  /**
   * Parses a given string and returns its value as a SFVec3f structure.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static SFVec3f getSFVec3fFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a SFVec2f structure.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static SFVec2f getSFVec2fFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a SFRotation structure.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static SFRotation getSFRotationFromString(const std::string &s);
  /**
   * Returns the string itself.
   * @param const std::string &s The string to be parsed.
   * @return The string itself.
   */
  static std::string getSFStringFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a SFColor structure.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static SFColor getSFColorFromString(const std::string &s);
  
  /**
   * Parses a given string and returns its value as a SFColor structure.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static SFColor getSFColorFromBytes(const std::string &s);
  
  /**
   * Parses a given string and returns its value as a SFColorRGBA structure.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static SFColorRGBA getSFColorRGBAFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a SFImage structure.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static SFImage getSFImageFromString(const std::string &s); 
  
  // Multi Field
  /**
   * Parses a given string and returns its value as a std::vector<float>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<float> getMFFloatFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a std::vector<int>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<int> getMFInt32FromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a std::vector<SFVec3f>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<SFVec3f> getMFVec3fFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a std::vector<SFVec2f>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<SFVec2f> getMFVec2fFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a std::vector<SFRotation>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<SFRotation> getMFRotationFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a std::vector<std::string>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<std::string> getMFStringFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a std::vector<SFColor>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<SFColor> getMFColorFromString(const std::string &s);
  /**
   * Parses a given string and returns its value as a std::vector<SFColorRGBA>.
   * @param const std::string &s The string to be parsed.
   * @return Value of the string.
   */
  static std::vector<SFColorRGBA> getMFColorRGBAFromString(const std::string &s);	
};

}

#endif

