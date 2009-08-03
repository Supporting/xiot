#include "X3DXMLLoader.h"
#include "X3DNodeHandler.h"
#include "X3DTypes.h"
#include "X3DSwitch.h"
#include "X3DXMLAttributes.h"
#include "X3DParseException.h"

#include <expat.h>

#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

namespace XIOT {

class X3DXMLContentHandler {
public:
  X3DXMLContentHandler(X3DNodeHandler* nodeHandler);
  ~X3DXMLContentHandler();

  virtual void startElement(const char* qName, const char** atts);
  virtual void endElement(const char* qName);

  void startDocument();
  void endDocument();

  //void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& exception);
  //void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& exception);

private:
  X3DNodeHandler* _nodeHandler;
  X3DSwitch _switch;
  int _skipCount;
};

void exp_startElement(void* data, const char* qName, const char** atts)
{
  X3DXMLContentHandler* p = reinterpret_cast<X3DXMLContentHandler*>(data);
  p->startElement(qName, atts);
} // startElement

void exp_endElement(void* data, const char* qName)
{
  X3DXMLContentHandler* p = reinterpret_cast<X3DXMLContentHandler*>(data);
  p->endElement(qName);
} // endElement


class XMLParserImpl 
{     
public:
  XMLParserImpl() : _parser(XML_ParserCreate(NULL)), _handler(NULL) {
	
    XML_SetElementHandler(_parser, exp_startElement, exp_endElement);
	
  };
  ~XMLParserImpl() {
	XML_ParserFree(_parser);
	if(_handler)
		delete _handler;
  }

  void setHandler(X3DXMLContentHandler* handler)
  {
	  X3DXMLContentHandler* oldHandler = _handler;
	  _handler = handler;
	  if(oldHandler)
		  delete oldHandler;
	  XML_SetUserData(_parser, reinterpret_cast<void*>(_handler));
  }

  XML_Parser _parser;
private:
  X3DXMLContentHandler* _handler;
};

/*class X3DXMLContentHandler
{
public: 
  X3DXMLContentHandler(X3DNodeHandler* nodeHandler);
  ~X3DXMLContentHandler();
  
  void startDocument();
  void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes &attrs);
  void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname);  
  void endDocument();
  void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& exception);
  void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& exception);
  
private:
  X3DNodeHandler* _nodeHandler;
  X3DSwitch _switch;
  int _skipCount;
};*/

X3DXMLContentHandler::X3DXMLContentHandler(X3DNodeHandler* nodeHandler) : _nodeHandler(nodeHandler), _skipCount(0)
{
	_switch.setNodeHandler(nodeHandler);
}

X3DXMLContentHandler::~X3DXMLContentHandler()
{
}

void X3DXMLContentHandler::startDocument()
{
	_nodeHandler->startDocument();
}

void X3DXMLContentHandler::startElement(const char* qName, const char** atts)
{
	if (_skipCount != 0)
	{
		_skipCount++;
		return;
	}
	int id = X3DTypes::getElementID(qName);
	X3DXMLAttributes xmlAttributes(atts);
	int state = _switch.doStartElement(id, xmlAttributes);
	if (state == XIOT::SKIP_CHILDREN)
		_skipCount = 1;

}

void X3DXMLContentHandler::endElement(const char* nodeName)
{
	if (_skipCount != 0)
	{
		_skipCount--;
		return;
	}
	int id = X3DTypes::getElementID(nodeName);
	_switch.doEndElement(id, nodeName);
}

void X3DXMLContentHandler::endDocument()
{
	_nodeHandler->endDocument();
}

/*void X3DXMLContentHandler::error(const SAXParseException& exception)
{
  char* message = XMLString::transcode(exception.getMessage());
  cerr << "XercesLoader::error: " << message << " at line: " << exception.getLineNumber() << endl;
}

void X3DXMLContentHandler::fatalError(const SAXParseException& exception)
{
  char* message = XMLString::transcode(exception.getMessage());
  throw X3DParseException(message, static_cast<int>(exception.getLineNumber()), static_cast<int>(exception.getColumnNumber()));
}*/

X3DXMLLoader::X3DXMLLoader()
{
  _impl = new XMLParserImpl();
 /* try 
  {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) 
  {
    char* message = XMLString::transcode(toCatch.getMessage());
    cerr << "XercesLoader::Error during initialization: " << message << endl;
    XMLString::release(&message);
  }*/
  
  
}

X3DXMLLoader::~X3DXMLLoader()
{
  delete _impl;
}

bool X3DXMLLoader::load(std::string fileName, bool fileValidation) const
{
	assert(_handler);


  _impl->setHandler(new X3DXMLContentHandler(_handler));
 
  std::fstream fin;
  fin.open(fileName.c_str(),std::ios::in);
  if( !fin.is_open() )
  {
	  cerr << "Could not open file: " << fileName << endl;
    return false;
  }

  _handler->startDocument();

  const int BUFF_SIZE = 10*1024;
  while(!fin.eof())
  {
    char* buffer = (char*)XML_GetBuffer(_impl->_parser, BUFF_SIZE);
    if(buffer == NULL)
    {
      cerr << "Could not acquire expat buffer" << endl;
      return false;
    } // if ...

    fin.read(buffer, BUFF_SIZE);
    if(XML_ParseBuffer(_impl->_parser, fin.gcount(), fin.eof()) == 0)
    {
      // error
      cerr << XML_ErrorString(XML_GetErrorCode(_impl->_parser)) << endl;
      return false;
    } // if ...
  } // while

  _handler->endDocument();
  return true;
}



}
