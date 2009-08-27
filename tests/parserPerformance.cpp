#include "Argument_helper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <xiot/X3DDefaultNodeHandler.h>
#include <xiot/X3DLoader.h>
#include <xiot/X3DTypes.h>
#include <xiot/X3DAttributes.h>


using namespace std;
using namespace XIOT;

string input_filename;
bool no_attributes, no_attribute_values;

class MyContentHandler : public X3DDefaultNodeHandler
{
	void startDocument()
	{
		cout << "Start Document" << endl;
	}

	void endDocument()
	{
		cout << "End Document" << endl;
	}

  int startUnhandled(const char* nodeName, const X3DAttributes &attr)
    {
    cout << "Handling: " << nodeName << endl;
    if (!no_attributes)
    {
      for(size_t i = 0; i < attr.getLength(); i++)
      {
        cout << " -- " <<  attr.getAttributeName(i) << endl;
        std::string s = attr.getAttributeValue(i);
      }
    }
    return CONTINUE;
    }
};


int start(const std::string &filename)
{
  X3DLoader l;
  MyContentHandler handler;
  l.setNodeHandler(&handler);
	
  
	try {
  time_t start,end;
  time(&start);
  for (int i = 0; i < 10; i++)
    {
    l.load(filename);
    }
  time(&end);
  double dif = difftime (end,start);
  printf ("Parsing took an average of %f seconds.\n", dif/10.0);

  }
  catch (std::exception& e)
	{
		cerr << endl << "Parsing failed: " << e.what() << endl;
	}
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

  ah.new_string("input_filename", "The name of the input file", input_filename);
  ah.new_flag('a', "skip-attributes", "Do not process attributes", no_attributes);
  ah.new_flag('b', "skip-attributes-values", "Do not process attribute values", no_attribute_values);

  //ARGUMENT_HELPER_BASICS(ah);
  ah.set_description("A simple test application for the parser performance");
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
