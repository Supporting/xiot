#include "Argument_helper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <xiot/FISAXParser.h>
#include <xiot/FIContentHandler.h>
#include <xiot/FIParserVocabulary.h>
#include <xiot/X3DParserVocabulary.h>

using namespace std;

string input_filename;
string output_filename;
bool attribute_index, element_index, attribute_values_index;

class MyContentHandler : public FI::DefaultContentHandler
{
	void startDocument()
	{
		cout << "Start Document" << endl;
	}

	void startElement(const FI::ParserVocabulary* vocab, const FI::Element &element, const FI::Attributes &attributes)
	{

    cout << "<";
    if (element_index)
      cout << element._qualifiedName._nameSurrogateIndex;  
    else
      cout << vocab->resolveElementName(element._qualifiedName);

    FI::Attributes::const_iterator I = attributes.begin();
    while(I!=attributes.end())
      {
      cout << " ";
      if (attribute_index)
        cout << (*I)._qualifiedName._nameSurrogateIndex;  
      else
        cout << vocab->resolveAttributeName((*I)._qualifiedName);

      if (attribute_values_index)
        {
        if ((*I)._normalizedValue._stringIndex != 0)
          {
          cout <<  "=" << (*I)._normalizedValue._stringIndex;
          }
        else if ((*I)._normalizedValue._characterString._encodingFormat == FI::ENCODINGFORMAT_ENCODING_ALGORITHM)
          {
          cout << "=ALG:" << (*I)._normalizedValue._characterString._encodingAlgorithm;
          }
        else
          cout << "=\"" << vocab->resolveAttributeValue((*I)._normalizedValue) << "\"";
        }
      else
        cout << "=\"" << vocab->resolveAttributeValue((*I)._normalizedValue) << "\"";
      I++;
      }
    cout <<">";
    }

	void characters(const FI::ParserVocabulary* vocab, const FI::CharacterChunk &chunk)
	{
		cout << vocab->resolveCharacterChunk(chunk.characterCodes);
	}


	void endElement(const FI::ParserVocabulary* vocab, const FI::Element &element)
	{
		cout << "</";
     if (element_index)
      cout << element._qualifiedName._nameSurrogateIndex;  
    else
      cout << vocab->resolveElementName(element._qualifiedName);
    cout << ">";
	}


	void endDocument()
	{
		cout << endl << "End Document" << endl;
	}

};


int start(string filename)
{
	FI::ContentHandler* handler = new MyContentHandler();
	FI::SAXParser parser;
	
	std::ifstream fs(filename.c_str(),  std::istream::binary |  std::istream::in);

	parser.setStream(&fs);
	parser.setContentHandler(handler);
	FI::ParserVocabulary* vocabulary = new XIOT::X3DParserVocabulary();
	// externalVocabularyURI = "urn:external-vocabulary"
	parser.addExternalVocabularies(vocabulary->getExternalVocabularyURI(), 	vocabulary);
	try {
		parser.parse();
	} catch (std::exception& e)
	{
		cerr << endl << "Parsing failed: " << e.what() << endl;
	}
	delete handler;
	return 0;
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

  ah.new_string("input_filename", "The name of the input file", input_filename);
  ah.new_flag('e', "element-as-index", "Print element index instead of name", element_index);
  ah.new_flag('a', "attribute-as-index", "Print attribute index instead of name", attribute_index);
  ah.new_flag('f', "attribute-values-as-index", "Print attribute index instead of name", attribute_values_index);
  //ah.new_string("output_filename", "The name of the output file", output_filename);
  
  //ARGUMENT_HELPER_BASICS(ah);
  ah.set_description("A simple test application for the FI parser");
  ah.set_author("Kristian Sons, kristian.sons@actor3d.com");
  ah.set_version(0.9f);
  ah.set_build_date(__DATE__);

  ah.process(argc, argv);


  // Check output string
  if (fileExists(input_filename))
  {
	  return start(input_filename);
  }
  
  cerr << "Input file not found or not readable: " << input_filename << endl;
  return 1;
}
