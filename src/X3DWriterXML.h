/*=========================================================================

/// This file is part of the X3DExporter library
/// Copyright (C) 2008 Kristian Sons (x3dexporter@actor3d.com)

/// This project has been funded with support from EDF R&D
/// Christophe MOUTON

/// TODO: Link

/// This library is free software; you can redistribute it and/or
/// modify it under the terms of the GNU Lesser General Public
/// License as published by the Free Software Foundation; either
/// version 2.1 of the License, or (at your option) any later version.

/// This library is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/// Lesser General Public License for more details.

/// You should have received a copy of the GNU Lesser General Public
/// License along with this library; if not, write to the Free Software
/// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

=========================================================================*/
#ifndef X3DWriterXML_H
#define X3DWriterXML_H

#include "X3DWriter.h"

struct XMLInfo {

XMLInfo(int _elementId)
{
  this->elementId = _elementId;
  this->endTagWritten = false;
}
  int elementId;
  bool endTagWritten;
};

class X3DLOADER_EXPORT X3DWriterXML : public X3DWriter
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
