/*=========================================================================
     This file is part of the XIOT library.

     Copyright (C) 2008-2009 EDF R&D
     Author: Kristian Sons (xiot@actor3d.com)

     This library is free software; you can redistribute it and/or modify
     it under the terms of the GNU Lesser Public License as published by
     the Free Software Foundation; either version 2.1 of the License, or
     (at your option) any later version.

     The XIOT library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser Public License for more details.

     You should have received a copy of the GNU Lesser Public License
     along with XIOT; if not, write to the Free Software
     Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
     MA 02110-1301  USA
=========================================================================*/
#ifndef X3DWRITERFI_H
#define X3DWRITERFI_H

#include <xiot/X3DWriter.h>
#include <xiot/X3DFIEncoder.h>

namespace XIOT {

class X3DWriterFIByte;
struct NodeInfo;

class XIOT_EXPORT X3DWriterFI : public X3DWriter
{
public:
    
  virtual void CloseFile();
  virtual int OpenFile(const char* file);


  //void Write(const char* str);

  virtual void Flush();

  void StartDocument();
  void EndDocument();

  // Elements
  void StartNode(int elementID);
  void EndNode();

  // Single Field
  virtual void SetSFFloat(int attributeID, float);
  virtual void SetSFInt32(int attributeID, int);
  virtual void SetSFBool(int attributeID, bool);

  virtual void SetSFVec3f(int attributeID, float x, float y, float z);
  virtual void SetSFVec2f(int attributeID, float s, float t);
  virtual void SetSFRotation(int attributeID, float x, float y, float z, float angle);
  virtual void SetSFString(int attributeID, const std::string &s);
  virtual void SetSFColor(int attributeID, float r, float g, float b);
  virtual void SetSFImage(int attributeID, const std::vector<int>&); 

  // Multi Field
  virtual void SetMFFloat(int attributeID, const std::vector<float>&);
  virtual void SetMFInt32(int attributeID, const std::vector<int>&);

  virtual void SetMFVec3f(int attributeID, const std::vector<float>&);
  virtual void SetMFVec2f(int attributeID, const std::vector<float>&);
  virtual void SetMFRotation(int attributeID, const std::vector<float>&);
  virtual void SetMFString(int attributeID, const std::vector<std::string>&);
  virtual void SetMFColor(int attributeID, const std::vector<float>&);

  int GetEncodingMethod();

  void SetFastest(int n_iFastest) {Fastest = n_iFastest;};

  X3DWriterFI();
  ~X3DWriterFI();


private:
  void StartAttribute(int attributeID, bool literal, bool addToTable = false);
  void EndAttribute();

  void CheckNode(bool callerIsAttribute = true);
  bool IsLineFeedEncodingOn;

  //int Depth;
  X3DWriterFIByte* Writer;
  std::vector<NodeInfo>* InfoStack;
  X3DFIEncoder _encoder;
  int Fastest;
  std::ofstream _stream;

};

} // namespace XIOT
#endif

