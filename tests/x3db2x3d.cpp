#include "Argument_helper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <xiot/FISAXParser.h>
#include <xiot/FIContentHandler.h>
#include <xiot/FIParserVocabulary.h>
#include <xiot/X3DParserVocabulary.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#endif


using namespace std;

string input_filename;
string output_filename;
bool attribute_index, element_index, attribute_values_index;

class Binary2TextHandler : public FI::DefaultContentHandler
{

public:
  void setStream(std::ostream& os) {
    _out=&os;
  }

private:
  std::ostream* _out;

  std::ostream& getStream() {
    return *_out;
  }

  std::string encodeForXml( const std::string &sSrc )
  {
    ostringstream sRet;

    for( string::const_iterator iter = sSrc.begin(); iter!=sSrc.end(); iter++ )
    {
      unsigned char c = (unsigned char)*iter;

      switch( c )
      {
      case '&': sRet << "&amp;"; break;
      case '<': sRet << "&lt;"; break;
      case '>': sRet << "&gt;"; break;
      case '"': sRet << "&quot;"; break;
      case '\'': sRet << "&apos;"; break;

      default:
        if ( c<32 || c>127 )
        {
          sRet << "&#" << (unsigned int)c << ";";
        }
        else
        {
          sRet << c;
        }
      }
    }

    return sRet.str();
  }

	void startDocument()
	{
    getStream() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	}

 	void endDocument()
	{
    getStream() << std::endl;
  }

	void startElement(const FI::ParserVocabulary* vocab, const FI::Element &element, const FI::Attributes &attributes)
	{
    std::ostream& os = getStream();
    os << "<";
    if (element_index)
      os << element._qualifiedName._nameSurrogateIndex;  
    else
      os << vocab->resolveElementName(element._qualifiedName);

    FI::Attributes::const_iterator I = attributes.begin();
    while(I!=attributes.end())
      {
      os << " ";
      if (attribute_index)
        os << (*I)._qualifiedName._nameSurrogateIndex;  
      else
        os << vocab->resolveAttributeName((*I)._qualifiedName);

      std::string attributeValue;
      
      if (attribute_values_index)
        {
        if ((*I)._normalizedValue._stringIndex != 0)
          {
          attributeValue = (*I)._normalizedValue._stringIndex;
          }
        else if ((*I)._normalizedValue._characterString._encodingFormat == FI::ENCODINGFORMAT_ENCODING_ALGORITHM)
          {
          attributeValue = (*I)._normalizedValue._characterString._encodingAlgorithm;
          }
        else
          attributeValue = vocab->resolveAttributeValue((*I)._normalizedValue);
        }
      else
          attributeValue = vocab->resolveAttributeValue((*I)._normalizedValue);

      os << "=\"" << encodeForXml(attributeValue) << "\"";
      I++;
      }
    os << ">";
    }

	void characters(const FI::ParserVocabulary* vocab, const FI::CharacterChunk &chunk)
	{
		getStream() << vocab->resolveCharacterChunk(chunk.characterCodes);
	}


	void endElement(const FI::ParserVocabulary* vocab, const FI::Element &element)
	{
		  std::ostream& os = getStream();
      os << "</";
      if (element_index)
        os << element._qualifiedName._nameSurrogateIndex;  
      else
        os << vocab->resolveElementName(element._qualifiedName);
      os << ">";
	}

};

int start(std::istream& in, std::ostream& out)
{
	Binary2TextHandler* handler = new Binary2TextHandler();
  handler->setStream(out);

	FI::SAXParser parser;
  parser.setStream(&in);

 	parser.setContentHandler(handler);
	FI::ParserVocabulary* vocabulary = new XIOT::X3DParserVocabulary();
	parser.addExternalVocabularies(vocabulary->getExternalVocabularyURI(), 	vocabulary);

  try {
		parser.parse();
	} catch (std::exception& e)
	{
		cerr << endl << "Parsing failed: " << e.what() << endl;
    return 1;
	}
  return 0;
}

int start(std::string inFile, std::string outFile)
{
  std::ifstream in;
  std::ofstream out;
  bool useStdIn = false;
  bool useStdOut = false;

  if (inFile.empty())
  {
    #ifdef _WIN32
      if (_setmode(STDIN_FILENO, _O_BINARY) == -1) {
        cerr << "ERROR: while converting cin to binary:" << strerror(errno) << endl;
        return 1;
      }
    #endif
    useStdIn = true;
  }
  else {
    in.open(inFile.c_str(),  std::istream::in |  std::istream::binary);
  }

  if (outFile.empty())
  {
    useStdOut = true;
  }
  else {
    out.open(outFile.c_str(), std::istream::out);
  }
  
  int retValue = start(useStdIn ? std::cin : in, useStdOut ? std::cout : out);

  if (!useStdOut)
    out.close();
  if (!useStdIn)
    in.close();

  return retValue;
 
}

bool fileExists(const std::string& fileName)
{
  std::fstream fin;
  fin.open(fileName.c_str(),std::ios::in);
  if( fin.is_open() )
  {
    fin.close();
    return true;
  }
  fin.close();
  return false;
}




int main(int argc, char *argv[])
{
  dsr::Argument_helper ah;
  element_index = attribute_index = attribute_values_index = false;

  ah.new_optional_string("input_filename", "The name of the input file", input_filename);
  ah.new_flag('e', "element-as-index", "Print element index instead of name", element_index);
  ah.new_flag('a', "attribute-as-index", "Print attribute index instead of name", attribute_index);
  ah.new_flag('f', "attribute-values-as-index", "Print attribute index instead of name", attribute_values_index);
  ah.new_named_string('o', "output", "output", "The name of the output file", output_filename);
  
  //ARGUMENT_HELPER_BASICS(ah);
  ah.set_description("Converts a binary X3D file to it's text representation.");
  ah.set_author("Kristian Sons, kristian.sons@supporting.com");
  ah.set_version(0.95f);
  ah.set_build_date(__DATE__);

  ah.process(argc, argv);

  // Check output string
  if (input_filename.empty() || fileExists(input_filename))
  {
    return start(input_filename, output_filename);
  }
  
  cerr << "Input file not found or not readable: " << input_filename << endl;
  return 1;
}
