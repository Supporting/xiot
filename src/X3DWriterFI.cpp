#include <xiot/X3DWriterFI.h>

#include <cstring>

#include <xiot/X3DTypes.h>
#include <xiot/FIEncodingAlgorithms.h>
#include <xiot/X3DFIEncodingAlgorithms.h>

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

//----------------------------------------------------------------------------
X3DWriterFI::~X3DWriterFI()
{
  delete this->_infoStack;
}

//----------------------------------------------------------------------------
X3DWriterFI::X3DWriterFI()
{
  this->_infoStack = new std::vector<NodeInfo>;
  this->_isLineFeedEncodingOn = true;
  this->_fastest = 0;
  this->type = X3DFI;
  this->_encoder.setStream(_stream);
  X3DTypes::initMaps();
}

//----------------------------------------------------------------------------
bool X3DWriterFI::setProperty(const char* const name, void* value)
{
	if (name == Property::FloatEncodingAlgorithm)
	{
		if (value == Encoder::BuiltIn)
			_encoder.setFloatAlgorithm(FI::FloatEncodingAlgorithm::ALGORITHM_ID);
		else if (value == Encoder::QuantizedzlibFloatArrayEncoder)
			_encoder.setFloatAlgorithm(QuantizedzlibFloatArrayAlgorithm::ALGORITHM_ID);
		else
			return false;
		
		return true;
	}
	else if (name == Property::IntEncodingAlgorithm)
	{
		if (value == Encoder::BuiltIn)
			_encoder.setIntAlgorithm(FI::IntEncodingAlgorithm::ALGORITHM_ID);
		else if (value == Encoder::DeltazlibIntArrayEncoder)
			_encoder.setIntAlgorithm(DeltazlibIntArrayAlgorithm::ALGORITHM_ID);
		else
			return false;
		return true;
	}
	return false;
}


//----------------------------------------------------------------------------
void* X3DWriterFI::getProperty(const char* const name) const
{
	if (name == Property::FloatEncodingAlgorithm)
	{
		int algorithmID = _encoder.getFloatAlgorithm();
		
		if (algorithmID == FI::FloatEncodingAlgorithm::ALGORITHM_ID)
			return (void*)Encoder::BuiltIn;
		if (algorithmID == QuantizedzlibFloatArrayAlgorithm::ALGORITHM_ID)
			return (void*)Encoder::QuantizedzlibFloatArrayEncoder;
	
		return NULL;
	}
	else if (name == Property::IntEncodingAlgorithm)
	{
		int algorithmID = _encoder.getIntAlgorithm();
		
		if (algorithmID == FI::IntEncodingAlgorithm::ALGORITHM_ID)
			return (void*)Encoder::BuiltIn;
		if (algorithmID == DeltazlibIntArrayAlgorithm::ALGORITHM_ID)
			return (void*)Encoder::DeltazlibIntArrayEncoder;
		
		return NULL;		
	}
	return 0;
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
  if (!this->_infoStack->empty())
    {
    this->checkNode(false);
    if (this->_isLineFeedEncodingOn)
      {
        _encoder.encodeLineFeed();
      }
     _encoder.fillByte();
    }

  this->_infoStack->push_back(NodeInfo(elementID));

  // ITU C.3.7.2: element is present
  _encoder.putBit(0);
}

//----------------------------------------------------------------------------
void X3DWriterFI::EndNode()
{
  assert(!this->_infoStack->empty());
  this->checkNode(false);
  if (this->_isLineFeedEncodingOn)
    {
    _encoder.encodeLineFeed();
    }
  if(!this->_infoStack->back().attributesTerminated)
    {
    //cout << "Terminated in EndNode: could be wrong" << endl;
    // ITU C.3.6.2: End of attribute
    _encoder.putBits("1111");
    }
  // ITU C.3.8: The four bits '1111' (termination) are appended.
  _encoder.putBits("1111");
  this->_infoStack->pop_back();
}

//----------------------------------------------------------------------------
void X3DWriterFI::checkNode(bool callerIsAttribute)
{
  if(!this->_infoStack->back().isChecked)
    {
    if (callerIsAttribute) // Element has attributes
      {
      // ITU C.3.3: then the bit '1' (presence) is appended
      _encoder.putBit(1);
      this->_infoStack->back().attributesTerminated = false;
      }
    else // Element has no attributes
      {
      // ITU C.3.3: otherwise, the bit '0' (absence) is appended
      _encoder.putBit(0);
      }
    // Write Node name (starting at third bit)
    // ITU: C.18.4 If the alternative name-surrogate-index is present, 
    // it is encoded as described in C.27.
    _encoder.encodeInteger3(this->_infoStack->back().nodeId + 1);
    this->_infoStack->back().isChecked = true;
    }
  // Element has attributes and childs
  else if (!callerIsAttribute && !this->_infoStack->back().attributesTerminated)
    {
    // ITU C.3.6.2: End of attribute
    _encoder.putBits("1111");
    this->_infoStack->back().attributesTerminated = true;
    }
}

//----------------------------------------------------------------------------
void X3DWriterFI::startAttribute(int attributeID, bool literal, bool addToTable)
{
  this->checkNode();
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
void X3DWriterFI::endAttribute()
{
	// Nothign to be done here
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFVec3f(int attributeID, float x, float y, float z)
{
  std::ostringstream ss;

  this->startAttribute(attributeID, true, false);

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

  this->startAttribute(attributeID, true, false);

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

  this->startAttribute(attributeID, true, false);

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
  this->startAttribute(attributeID, true, false);
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
  this->startAttribute(attributeID, true, false);
  _encoder.encodeAttributeIntegerArray(&values.front(), values.size());
}


//----------------------------------------------------------------------------
void X3DWriterFI::SetSFInt32(int attributeID, int iValue)
{
  std::ostringstream ss;
  this->startAttribute(attributeID, true, false);

  // Xj3D writes out single value fields in string encoding. Expected:
  //FIEncoderFunctions::EncodeFloatFI<float>(this->Writer, &value, 1);
  ss << iValue;
  _encoder.encodeCharacterString3(ss.str());
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetMFInt32(int attributeID, const std::vector<int>& values)
{
  this->startAttribute(attributeID, true, false);
  _encoder.encodeAttributeIntegerArray(&values.front(), values.size());
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFFloat(int attributeID, float fValue)
{
  std::ostringstream ss;

  this->startAttribute(attributeID, true, false);

  // Xj3D writes out single value fields in string encoding. Expected:
  //FIEncoderFunctions::EncodeFloatFI<float>(this->Writer, &value, 1);
  ss << fValue;
  _encoder.encodeCharacterString3(ss.str());
}

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFBool(int attributeID, bool bValue)
{
  this->startAttribute(attributeID, false);
  _encoder.encodeInteger2(bValue ? 2 : 1);
}

//----------------------------------------------------------------------------
/*void X3DWriterFI::SetMFBool(int attributeID, std::vector<bool>& values)
{
  // ok?
  this->startAttribute(attributeID, false);

  // TODO: Implement standard FI boolean encoder (s FI 10.7)
  for(unsigned int i = 0; i < 1; i++)
    _encoder.EncodeInteger2(this->Writer, values[i] ? 2 : 1);

}*/

//----------------------------------------------------------------------------
void X3DWriterFI::SetSFString(int attributeID, const std::string &s)
{
  this->startAttribute(attributeID, true, true);
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
  this->startAttribute(attributeID, true, true);
  _encoder.encodeCharacterString3(sTemp.str());
}


//----------------------------------------------------------------------------
void X3DWriterFI::Flush()
{

}

} // namespace XIOT