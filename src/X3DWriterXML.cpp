#include <xiot/X3DWriterXML.h>

#include <cstring>

#include <xiot/X3DTypes.h>

using namespace XIOT;

X3DWriterXML::~X3DWriterXML()
{
  delete this->InfoStack;
}
 
//-----------------------------------------------------------------------------
X3DWriterXML::X3DWriterXML()
{
  this->InfoStack = new std::vector<XMLInfo>;
  this->Depth = 0;
  this->ActTab = "";
  this->type = X3DXML;
  this->OutputStream = NULL;
  X3DTypes::initMaps();
}


int X3DWriterXML::OpenFile(const char* file)
{
  this->CloseFile();
  this->OutputStream = fopen(file, "w");
  
  return ((this->OutputStream == NULL) ? 0 : 1);
}

//----------------------------------------------------------------------------
void X3DWriterXML::CloseFile()
{
  if (this->OutputStream)
    {
    fclose(OutputStream);
    }
}

//-----------------------------------------------------------------------------
void X3DWriterXML::StartDocument()
{
  this->Depth = 0;
  fprintf(this->OutputStream, "<?xml version=\"1.0\" encoding =\"UTF-8\"?>\n\n");
}

//-----------------------------------------------------------------------------
void X3DWriterXML::EndDocument()
{
  assert(this->Depth == 0);
  
}

//-----------------------------------------------------------------------------
void X3DWriterXML::StartNode(int elementID)
{
  // End last tag, if this is the first child
  if (!this->InfoStack->empty())
    {
    if (!this->InfoStack->back().endTagWritten)
      {
		  fprintf(this->OutputStream, ">\n");
      this->InfoStack->back().endTagWritten = true;
      }
    }

  this->InfoStack->push_back(XMLInfo(elementID));
   
  fprintf(this->OutputStream, "%s<%s", this->ActTab.c_str(), X3DTypes::getElementByID(elementID));
  this->AddDepth();
}

//-----------------------------------------------------------------------------
void X3DWriterXML::EndNode()
{
  assert(!this->InfoStack->empty());
  this->SubDepth();
  int elementID = this->InfoStack->back().elementId;

  // There were no childs
  if (!this->InfoStack->back().endTagWritten)
    {
	fprintf(this->OutputStream, "/>\n");
    }
  else
    {
	fprintf(this->OutputStream, "%s</%s>\n", this->ActTab.c_str(), X3DTypes::getElementByID(elementID));
    }

  this->InfoStack->pop_back();
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFFloat(int attributeID, float fValue)
{
  fprintf(this->OutputStream, " %s=\"%g\"", X3DTypes::getAttributeByID(attributeID), fValue);
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFInt32(int attributeID, int iValue)
{
  fprintf(this->OutputStream, " %s=\"%i\"", X3DTypes::getAttributeByID(attributeID), iValue);
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFBool(int attributeID, bool bValue)
{
	fprintf(this->OutputStream, " %s=\"%s\"", X3DTypes::getAttributeByID(attributeID), (bValue ? "true" : "false"));
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFVec3f(int attributeID, float x, float y, float z)
{
  fprintf(this->OutputStream, " %s=\"%g %g %g\"", X3DTypes::getAttributeByID(attributeID), x, y, z);

}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFVec2f(int attributeID, float s, float t)
{
  fprintf(this->OutputStream, " %s=\"%g %g\"", X3DTypes::getAttributeByID(attributeID), s, t);
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFImage(int attributeID, const std::vector<int>& values)
{
  fprintf(this->OutputStream, " %s=\"\n%s", X3DTypes::getAttributeByID(attributeID), this->ActTab.c_str());

  unsigned int i = 0;
  
  assert(values.size() > 2);
  char buffer[20];
  //this->OutputStream << values[0] << " "; // width
  //this->OutputStream << values[1] << " "; // height
  //int bpp = values[2]; this->OutputStream << bpp << "\n"; // bpp
  fprintf(this->OutputStream, "%i %i %i\n", values[0], values[1], values[2]);
    
  i = 3;
  unsigned int j = 0;

  while (i < values.size())
  {
    sprintf(buffer,"0x%.8x",values[i]);
	fwrite(buffer, 1, strlen(buffer), this->OutputStream);
      
    if (j%(8*values[2]))
    {
      fprintf(this->OutputStream, " ");
    }
    else
    {
      fprintf(this->OutputStream, "\n");
    }
    i++; j+=values[2];
  }
  
  fprintf(this->OutputStream, "\"");
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFColor(int attributeID, float r, float g, float b)
{
  this->SetSFVec3f(attributeID, r, g, b);
}

//-----------------------------------------------------------------------------
// wieso -angle?
void X3DWriterXML::SetSFRotation(int attributeID, float x, float y, float z, float angle)
{
  fprintf(this->OutputStream, " %s=\"%g %g %g %g\"", X3DTypes::getAttributeByID(attributeID), x, y, z, angle);
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetSFString(int attributeID, const std::string &s)
{
  fprintf(this->OutputStream, " %s=\"%s\"", X3DTypes::getAttributeByID(attributeID), s.c_str());
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetMFFloat(int attributeID, const std::vector<float>& values)
{
  fprintf(this->OutputStream, " %s=\"\n%s", X3DTypes::getAttributeByID(attributeID), this->ActTab.c_str());
  
  unsigned int i = 0;
  while (i < values.size())
    {
	fprintf(this->OutputStream, "%g", values[i]);
    if ((i+1)%3)
      {
      fprintf(this->OutputStream, " ");
      }
    else
      {
	  fprintf(this->OutputStream, ",\n%s", this->ActTab.c_str());
      }
    i++;
    }
  fprintf(this->OutputStream, "\"");
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetMFRotation(int attributeID, const std::vector<float>& values)
{
  fprintf(this->OutputStream, " %s=\"\n%s", X3DTypes::getAttributeByID(attributeID), this->ActTab.c_str());

  unsigned int i = 0;
  while (i < values.size())
    {
    fprintf(this->OutputStream, "%g", values[i]);
    if ((i+1)%4)
      {
      fprintf(this->OutputStream, " ");
      }
    else
      {
	  fprintf(this->OutputStream, ",\n%s", this->ActTab.c_str());
      }
    i++;
    }
  fprintf(this->OutputStream, "\"");

}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetMFColor(int attributeID, const std::vector<float>& values)
{
  this->SetMFFloat(attributeID, values);
}


//-----------------------------------------------------------------------------
void X3DWriterXML::SetMFInt32(int attributeID, const std::vector<int>& values)
{
  fprintf(this->OutputStream, " %s=\"\n%s", X3DTypes::getAttributeByID(attributeID), this->ActTab.c_str());
  
  unsigned int i = 0;
  while (i < values.size())
    {
	fprintf(this->OutputStream, "%i ", values[i]);
    if (values[i] == -1) 
      {
	  fprintf(this->OutputStream, "\n%s", this->ActTab.c_str());
      }
    i++;
    }
  fprintf(this->OutputStream, "\"");
}

// Not implemented for FI encoding yet
/*void X3DWriterXML::SetMFBool(int attributeID, std::vector<bool>& values)
{
  fprintf(this->OutputStream, " %s=\"", X3DTypes::getAttributeByID(attributeID));
  for(unsigned int i = 0; i < values.size(); i++)
  {
	if (i != 0)
		fprintf(this->OutputStream, " ");
    if (values[i])
		fprintf(this->OutputStream, "true");
	else
		fprintf(this->OutputStream, "false");
  }
  fprintf(this->OutputStream, "\"");
}*/

//-----------------------------------------------------------------------------
void X3DWriterXML::SetMFVec2f(int attributeID, const std::vector<float>& values)
{
  fprintf(this->OutputStream, " %s=\"\n", X3DTypes::getAttributeByID(attributeID));
  
  assert((values.size() % 2) == 0);

  for(unsigned int i = 0; i < values.size(); i+=2)
  {
    fprintf(this->OutputStream, "%s%g %g,\n", this->ActTab.c_str(), values[i], values[i+1]);
  }

  fprintf(this->OutputStream, "%s\"", this->ActTab.c_str());
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetMFVec3f(int attributeID, const std::vector<float>& values)
{
  fprintf(this->OutputStream, " %s=\"\n", X3DTypes::getAttributeByID(attributeID));
  
  assert((values.size() % 3) == 0);

  for(unsigned int i = 0; i < values.size(); i+=3)
  {
	fprintf(this->OutputStream, "%s%g %g %g,\n", this->ActTab.c_str(), values[i], values[i+1], values[i+2]);
  }

  fprintf(this->OutputStream, "%s\"", this->ActTab.c_str());
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SetMFString(int attributeID, const std::vector<std::string>& strings)
{
  fprintf(this->OutputStream, " %s='", X3DTypes::getAttributeByID(attributeID));

  for(unsigned int i = 0; i < strings.size(); i++)
  {
	fprintf(this->OutputStream, "\"%s\"", strings[i].c_str());
	if(i < (strings.size() - 1))
		fprintf(this->OutputStream, " ");
  }
  
  fprintf(this->OutputStream, "'");
}

//-----------------------------------------------------------------------------
void X3DWriterXML::Flush()
{
  fflush(this->OutputStream);
}

//-----------------------------------------------------------------------------
void X3DWriterXML::AddDepth()
{
  this->ActTab += "  ";
}

//-----------------------------------------------------------------------------
void X3DWriterXML::SubDepth()
{
  this->ActTab.erase(0, 2);
}

void X3DWriterXML::printAttributeString(int attributeID)
{
  //this->OutputStream << " " << X3DTypes::getAttributeByID(attributeID) << "=\"" << this->GetNewline() << this->ActTab;
  //fprintf(this->OutputStream, " %s=\"\n%s", this->ActTab);
}
