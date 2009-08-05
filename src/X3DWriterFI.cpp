#include <xiot/X3DWriterFI.h>

#include <cstring>

#include <xiot/X3DZLibDataCompressor.h>
#include <xiot/X3DTypes.h>

//#define ENCODEASSTRING 1

using namespace std;

namespace XIOT {

/*======================================================================== */
struct NodeInfo 
{
  NodeInfo(int _nodeId)
    {
    this->nodeId = _nodeId;
    this->isChecked = false;
    this->attributesTerminated = true;
    }
  int nodeId;
  bool attributesTerminated;
  bool isChecked;
};

/*======================================================================== */
/*class X3DWriterFIByte
{
public:
  ~X3DWriterFIByte();
  X3DWriterFIByte() {};
  // This is the current byte to fill
  unsigned char CurrentByte;
  // This is the current byte position. Range: 0-7
  unsigned char CurrentBytePos;

  // Opens the specified file in binary mode. Returns 0
  // if failed
  int OpenFile(const char* file);

  // Puts a bitstring to the current byte bit by bit
  void PutBits(const std::string &bitstring);
  // Puts the integer value to the stream using count bits
  // for encoding
  void PutBits(unsigned int value, unsigned char count);
  // Puts on bit to the current byte true = 1, false = 0
  void PutBit(bool on);
  // Puts whole bytes to the file stream. CurrentBytePos must
  // be 0 for this
  void PutBytes(const char* bytes, size_t length);
  // Fills up the current byte with 0 values
  void FillByte();

private:
  unsigned char Append(unsigned int value, unsigned char count);
  void TryFlush();
  std::ofstream FileStream;

  X3DWriterFIByte(const X3DWriterFIByte&); // Not implemented
  void operator=(const X3DWriterFIByte&); // Not implemented
};

//----------------------------------------------------------------------------
X3DWriterFIByte::~X3DWriterFIByte()
{
  if (this->FileStream.is_open())
    {
    this->FileStream.close();
    }
}

//----------------------------------------------------------------------------
int X3DWriterFIByte::OpenFile(const char* file)
{
  this->CurrentByte = 0;
  this->CurrentBytePos = 0;
  this->FileStream.open (file, ios::out | ios::binary);
  return this->FileStream.fail() ? 0 : 1;
}

//----------------------------------------------------------------------------
void X3DWriterFIByte::TryFlush()
{
  if (this->CurrentBytePos == 8)
    {
    this->FileStream.write((char*)(&(this->CurrentByte)), 1);
    this->CurrentByte = 0;
    this->CurrentBytePos = 0;
    }
}

//----------------------------------------------------------------------------
void X3DWriterFIByte::FillByte()
{
  while (this->CurrentBytePos !=0)
    {
    this->PutBit(0);
    }
}

//----------------------------------------------------------------------------
void X3DWriterFIByte::PutBit(bool on)
{
  assert(this->CurrentBytePos < 8);
  if (on)
    {
    unsigned char pos = this->CurrentBytePos;
    unsigned char mask = (unsigned char)(0x80 >> pos);
    this->CurrentByte |= mask;
    }
  this->CurrentBytePos++;
  TryFlush();
}

//----------------------------------------------------------------------------
unsigned char X3DWriterFIByte::Append(unsigned int value, unsigned char count)
{
  assert(this->CurrentBytePos < 8);
  while ((this->CurrentBytePos < 8) && count > 0)
    {
    // Value and der Stelle i
    unsigned int mask = 1;
    bool isSet = !(((mask << (count - 1)) & value) == 0);
    if (isSet)
      {
      this->CurrentByte |= static_cast<unsigned char>(0x80 >> this->CurrentBytePos);
      }
    this->CurrentBytePos++;
    count--;
    }
  TryFlush();
  return count;
}

//----------------------------------------------------------------------------
void X3DWriterFIByte::PutBytes(const char* bytes, size_t length)
{
  if(this->CurrentBytePos == 0)
    {
    FileStream.write(bytes, length);
    }
  else
    {
    std::cerr << "Wrong position in X3DWriterFIByte::PutBytes" << std::endl;
    assert(false);
    }
}

//----------------------------------------------------------------------------
void X3DWriterFIByte::PutBits(unsigned int value, unsigned char count)
{
  // Can be optimized
  while (count > 0)
    {
    count = this->Append(value, count);
    }
}


//----------------------------------------------------------------------------
void X3DWriterFIByte::PutBits(const std::string &bitstring)
{
  std::string::const_iterator I = bitstring.begin();
  while(I != bitstring.end())
    {
    this->PutBit((*I) == '1');
    I++;
    }
}*/

//#include <xiot/X3DWriterFIHelper.h>

//----------------------------------------------------------------------------
X3DWriterFI::~X3DWriterFI()
{
  delete this->InfoStack;
}

//----------------------------------------------------------------------------
X3DWriterFI::X3DWriterFI()
{
  this->InfoStack = new std::vector<NodeInfo>;
  this->Writer = NULL;
  this->IsLineFeedEncodingOn = true;
  this->Fastest = 0;
  this->type = X3DFI;
  this->_encoder.setStream(_stream);
  X3DTypes::initMaps();
}

//----------------------------------------------------------------------------
int X3DWriterFI::OpenFile(const char* file)
{
  this->CloseFile();
  
  _stream.open (file, ios::out | ios::binary);
  if(!_stream.fail())
  {
	  _encoder.reset();
	  return 1;
  }
  return 0;
}

//----------------------------------------------------------------------------
void X3DWriterFI::CloseFile()
{
  if (_stream.is_open())
	_stream.close();
}

//----------------------------------------------------------------------------
void X3DWriterFI::StartDocument()
{
  _encoder.reset();
  _encoder.encodeHeader(false);
  _encoder.encodeInitialVocabulary();
}

//----------------------------------------------------------------------------
void X3DWriterFI::EndDocument()
{
  _encoder.encodeDocumentTermination();
}


//----------------------------------------------------------------------------
void X3DWriterFI::StartNode(int elementID)
{
  if (!this->InfoStack->empty())
    {
    this->CheckNode(false);
    if (this->IsLineFeedEncodingOn)
      {
        _encoder.encodeLineFeed();
      }
     _encoder.fillByte();
    }

  this->InfoStack->push_back(NodeInfo(elementID));

  // ITU C.3.7.2: element is present
  _encoder.putBit(0);
}

//----------------------------------------------------------------------------
void X3DWriterFI::EndNode()
{
  assert(!this->InfoStack->empty());
  this->CheckNode(false);
  if (this->IsLineFeedEncodingOn)
    {
    _encoder.encodeLineFeed();
    }
  if(!this->InfoStack->back().attributesTerminated)
    {
    //cout << "Terminated in EndNode: could be wrong" << endl;
    // ITU C.3.6.2: End of attribute
    _encoder.putBits("1111");
    }
  // ITU C.3.8: The four bits '1111' (termination) are appended.
  _encoder.putBits("1111");
  this->InfoStack->pop_back();
}

//----------------------------------------------------------------------------
void X3DWriterFI::CheckNode(bool callerIsAttribute)
{
  if(!this->InfoStack->back().isChecked)
    {
    if (callerIsAttribute) // Element has attributes
      {
      // ITU C.3.3: then the bit '1' (presence) is appended
      _encoder.putBit(1);
      this->InfoStack->back().attributesTerminated = false;
      }
    else // Element has no attributes
      {
      // ITU C.3.3: otherwise, the bit '0' (absence) is appended
      _encoder.putBit(0);
      }
    // Write Node name (starting at third bit)
    // ITU: C.18.4 If the alternative name-surrogate-index is present, 
    // it is encoded as described in C.27.
    _encoder.encodeInteger3(this->InfoStack->back().nodeId + 1);
    this->InfoStack->back().isChecked = true;
    }
  // Element has attributes and childs
  else if (!callerIsAttribute && !this->InfoStack->back().attributesTerminated)
    {
    // ITU C.3.6.2: End of attribute
    _encoder.putBits("1111");
    this->InfoStack->back().attributesTerminated = true;
    }
}

//----------------------------------------------------------------------------
void X3DWriterFI::StartAttribute(int attributeID, bool literal, bool addToTable)
{
  this->CheckNode();
  // ITU C.3.6.1: Start of attribute
  _encoder.putBit(0);
  // ITU C.4.3 The value of qualified-name is encoded as described in C.17.
  _encoder.encodeInteger2(attributeID +1);

  // ITU C.14.3: If the alternative literal-character-string is present,
  //then the bit '0' (discriminant) is appended
  // ITU C.14.4: If the alternative string-index is present, 
  // then the bit '1' (discriminant) is appended
  _encoder.putBit(literal ? 0 : 1);
  if (literal)
    {
    // ITU C.14.3.1 If the value of the component add-to-table is TRUE, 
    // then the bit '1' is appended to the bit stream;
    _encoder.putBit(addToTable);
    }
}

//----------------------------------------------------------------------------
void X3DWriterFI::EndAttribute()
{
	// Nothign to be done here
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFVec3f(int attributeID, float x, float y, float z)
{
  std::ostringstream ss;

  this->StartAttribute(attributeID, true, false);

#ifdef ENCODEASSTRING
  size_t size = 3;
  float temp[3];
  temp[0] = x;
  temp[1] = y;
  temp[2] = z;

  _encoder.EncodeFloatFI(this->Writer, temp, size);

#else
  ss << x << " " << y << " " << z;
  _encoder.encodeCharacterString3(ss.str());
#endif
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFVec2f(int attributeID, float s, float t)
{
  std::ostringstream ss;

  this->StartAttribute(attributeID, true, false);

#ifdef ENCODEASSTRING
  size_t size = 2;
  float temp[2];
  temp[0] = s;
  temp[1] = t;
  
  _encoder.EncodeFloatFI(this->Writer, temp, size);

#else
  ss << s << " " << t;
  _encoder.encodeCharacterString3(ss.str());
#endif
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFColor(int attributeID, float r, float g, float b)
{
  this->SetSFVec3f(attributeID, r, g, b);
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFRotation(int attributeID, float x, float y, float z, float angle)
{
  std::ostringstream ss;

  this->StartAttribute(attributeID, true, false);

#ifdef ENCODEASSTRING
  size_t size = 4;
  float temp[4];
  
  size = 4;
  temp[0] = x;
  temp[1] = y;
  temp[2] = z;
  temp[3] = -angle * FLOAT_DEGTORAD;
    
  _encoder.EncodeFloatFI(this->Writer, temp, size);

#else
  ss << x << " " << y << " " << z << " " << angle;
  
  _encoder.encodeCharacterString3(ss.str());

#endif
}

void X3DWriterFI::SetMFFloat(int attributeID, const std::vector<float>& values)
{
  this->StartAttribute(attributeID, true, false);
  _encoder.encodeAttributeFloatArray(&(values.front()), values.size());
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetMFColor(int attributeID, const std::vector<float>& values)
{
  this->SetMFFloat(attributeID, values);
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetMFRotation(int attributeID, const std::vector<float>& values)
{
#ifdef ENCODEASSTRING
  std::ostringstream ss;
  unsigned int i = 0;
    while (i < values.size())
      {
      ss << values[i++] << " " << values[i++] << " " << values[i++] << " " << values[i++] << ",";
      }

    _encoder.EncodeCharacterString3(this->Writer, ss.str());
#else
  this->SetMFFloat(attributeID, values);
#endif
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetMFVec3f(int attributeID, const std::vector<float>& values)
{
  this->SetMFFloat(attributeID, values);
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetMFVec2f(int attributeID, const std::vector<float>& values)
{
#ifdef ENCODEASSTRING
  std::ostringstream ss;
  unsigned int i = 0;
    while (i < values.size())
      {
      ss << values[i++] << " " << values[i++] << ",";
      }

    _encoder.EncodeCharacterString3(this->Writer, ss.str());
#else
  this->SetMFFloat(attributeID, values);
#endif
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFImage(int attributeID, const std::vector<int>& values)
{
  this->StartAttribute(attributeID, true, false);
  _encoder.encodeAttributeIntegerArray(&values.front(), values.size());
}


//----------------------------------------------------------------------------
void X3DWriterFI::SetSFInt32(int attributeID, int iValue)
{
  std::ostringstream ss;
  this->StartAttribute(attributeID, true, false);

  // Xj3D writes out single value fields in string encoding. Expected:
  //FIEncoderFunctions::EncodeFloatFI<float>(this->Writer, &value, 1);
  ss << iValue;
  _encoder.encodeCharacterString3(ss.str());
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetMFInt32(int attributeID, const std::vector<int>& values)
{
  this->StartAttribute(attributeID, true, false);
  _encoder.encodeAttributeIntegerArray(&values.front(), values.size());
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFFloat(int attributeID, float fValue)
{
  std::ostringstream ss;

  this->StartAttribute(attributeID, true, false);

  // Xj3D writes out single value fields in string encoding. Expected:
  //FIEncoderFunctions::EncodeFloatFI<float>(this->Writer, &value, 1);
  ss << fValue;
  _encoder.encodeCharacterString3(ss.str());
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFBool(int attributeID, bool bValue)
{
  this->StartAttribute(attributeID, false);
  _encoder.encodeInteger2(bValue ? 2 : 1);
}

//----------------------------------------------------------------------------
/*void X3DWriterFI::SetMFBool(int attributeID, std::vector<bool>& values)
{
  // ok?
  this->StartAttribute(attributeID, false);

  // TODO: Implement standard FI boolean encoder (s FI 10.7)
  for(unsigned int i = 0; i < 1; i++)
    _encoder.EncodeInteger2(this->Writer, values[i] ? 2 : 1);

}*/

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFString(int attributeID, const std::string &s)
{
  this->StartAttribute(attributeID, true, true);
  _encoder.encodeCharacterString3(s);
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetMFString(int attributeID, const std::vector<std::string>& strings)
{
  std::ostringstream sTemp;
  
  for(unsigned int i = 0; i < strings.size(); i++)
  {
    sTemp << '"' << strings[i] << '"';
	if(i < (strings.size() - 1))
		sTemp << " ";
  }
  this->StartAttribute(attributeID, true, true);
  _encoder.encodeCharacterString3(sTemp.str());
}


//----------------------------------------------------------------------------
void X3DWriterFI::Flush()
{

}

} // namespace XIOT