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
#ifndef X3DWriterXML_H
#define X3DWriterXML_H

#include <xiot/X3DWriter.h>

struct XMLInfo {

XMLInfo(int _elementId)
{
  this->elementId = _elementId;
  this->endTagWritten = false;
}
  int elementId;
  bool endTagWritten;
};

class XIOT_EXPORT X3DWriterXML : public X3DWriter
{

public:
    
  virtual void CloseFile();
  virtual int OpenFile(const char* file);
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
  

public:
  X3DWriterXML();
  ~X3DWriterXML();

private:
 
  const char* GetNewline() { return "\n"; };
  void AddDepth();
  void SubDepth();
  void printAttributeString(int attributeID);

  std::string ActTab;
  int Depth;
  FILE *OutputStream;
  std::vector<XMLInfo> *InfoStack;

};

#endif
