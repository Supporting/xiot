#include <xiot/X3DXMLLoader.h>
#include <xiot/X3DNodeHandler.h>
#include <xiot/X3DTypes.h>
#include <xiot/X3DSwitch.h>
#include <xiot/X3DXMLAttributes.h>
#include <xiot/X3DParseException.h>

#include <qxml.h>

#include <iostream>
#include <cassert>

using namespace std;

namespace XIOT {

class X3DXMLContentHandler;

class XMLParserImpl 
{     
public:
	XMLParserImpl() : _handler(NULL), _parser(NULL) {};
  
  QXmlSimpleReader *_parser;
  X3DXMLContentHandler *_handler;
};

class X3DXMLContentHandler : public QXmlDefaultHandler
{
public: 
  X3DXMLContentHandler(X3DNodeHandler* nodeHandler);
  ~X3DXMLContentHandler();
  
  bool startDocument();
  bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
  bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
  bool endDocument();
  bool fatalError(const QXmlParseException &exception);
  bool error(const QXmlParseException &exception);
  
private:
  X3DNodeHandler* _nodeHandler;
  X3DSwitch _switch;
  int _skipCount;
};

X3DXMLContentHandler::X3DXMLContentHandler(X3DNodeHandler* nodeHandler) : _nodeHandler(nodeHandler), _skipCount(0)
{
	_switch.setNodeHandler(nodeHandler);
}

X3DXMLContentHandler::~X3DXMLContentHandler()
{
}

bool X3DXMLContentHandler::startDocument()
{
	_nodeHandler->startDocument();
  return true;
}

bool X3DXMLContentHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
	if (_skipCount != 0)
	{
		_skipCount++;
		return true;
	}
  
  int id = X3DTypes::getElementID(qName.toAscii().constData());
	X3DXMLAttributes xmlAttributes(&atts);
	int state = _switch.doStartElement(id, xmlAttributes);
	if (state == XIOT::SKIP_CHILDREN)
		_skipCount = 1;
  return true;
}

bool X3DXMLContentHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
	if (_skipCount != 0)
	{
		_skipCount--;
		return true;
	}
  std::string nodeName(qName.toAscii().constData());
	int id = X3DTypes::getElementID(nodeName);
	_switch.doEndElement(id, nodeName.c_str());
  return true;
}

bool X3DXMLContentHandler::endDocument()
{
	_nodeHandler->endDocument();
  return true;
}

bool X3DXMLContentHandler::error(const QXmlParseException &exception)
{
  throw new X3DParseException(exception.message().toAscii().constData(), exception.lineNumber(), exception.columnNumber());
}

bool X3DXMLContentHandler::fatalError(const QXmlParseException& exception)
{
  throw new X3DParseException(exception.message().toAscii().constData(), exception.lineNumber(), exception.columnNumber());
}

X3DXMLLoader::X3DXMLLoader()
{
  _impl = new XMLParserImpl();
  
  _impl->_parser = new QXmlSimpleReader();
  

}

X3DXMLLoader::~X3DXMLLoader()
{
  delete _impl->_parser;
  if (_impl->_handler)
	  delete _impl->_handler;
  delete _impl;
}

bool X3DXMLLoader::load(const char* fileStr, bool fileValidation) const
{
	assert(_handler);


  _impl->_handler = new X3DXMLContentHandler(_handler);
  _impl->_parser->setContentHandler(_impl->_handler);
  _impl->_parser->setErrorHandler(_impl->_handler);
  _impl->_parser->setDTDHandler(_impl->_handler);

  QFile xmlFile(fileStr);
  QXmlInputSource source(&xmlFile);   
  
  try 
  {
    _impl->_parser->parse(source);
  }
  catch (...) 
  {
    cerr << "X3DXMLLoader::load: internal error." << endl;
    return false;
  }
  return true;
  
}



}
