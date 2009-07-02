#include "Argument_helper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include "X3DLoader.h"
#include "X3DDefaultNodeHandler.h"
#include "X3DAttributes.h"
#include "X3DParseException.h"
#include "X3DWriterFI.h"
#include "X3DWriterXML.h"


using namespace std;
using namespace XIOT;

string input_filename;
string output_filename;

class MyNodeHandler : public X3DDefaultNodeHandler
{
public:
	MyNodeHandler() {
		_eventCount = 0;
	}

	void testEvent(int number, const char* eventName)
	{
		_eventCount++;
		if (_eventCount != number)
		{
			cerr << eventName<< ": Not the expected event order. Expected: " << number << " is: " << _eventCount << endl;
			assert(false);
		}
		cout << "Event number " << number << ": " << eventName << endl;
	}

	virtual int startX3D(const X3DAttributes &attr)
	{
		testEvent(1, "start X3D");
		return CONTINUE;		
	}

	virtual int startShape(const X3DAttributes& attr)
	{
		testEvent(3, "start Shape");
		return CONTINUE;		
	}

	virtual int endShape()
	{
		testEvent(10, "end Shape");
		return CONTINUE;		
	}

	virtual int startBox(const X3DAttributes& attr)
	{
		testEvent(8, "start Box");
		int index = attr.getAttributeIndex(ID::size);
		assert(index != -1);
		
		return CONTINUE;		
	}
	
	virtual int endBox()
	{
		testEvent(9, "end Box");
		return CONTINUE;		
	}
	
	virtual int startMaterial(const X3DAttributes& attr)
	{
		testEvent(5, "start Material");
		int index = attr.getAttributeIndex(ID::diffuseColor);
		assert(index != -1);
		
		SFColor diffuseColor = attr.getSFColor(index);
		assert(diffuseColor.r == 1.0);
		assert(diffuseColor.g == 0.0);
		assert(diffuseColor.b == 0.0);
		cout << "Diffuse Color is: " << diffuseColor.r << " " << diffuseColor.g << " " << diffuseColor.b << endl;
		
		index = attr.getAttributeIndex(ID::transparency);
		assert(index != -1);
		float transparency = attr.getSFFloat(index);
		assert(transparency == 0.1f);
		
		return CONTINUE;		
	}

	virtual int startUnhandled(const char* nodeName, const X3DAttributes& attr)
	{
		cout << "Event number " << ++_eventCount << ": unhandled start event " << nodeName << endl;
		return CONTINUE;
	}

	virtual int endUnhandled(const char* nodeName)
	{
		cout << "Event number " << ++_eventCount << ": unhandled end event " << nodeName << endl;
		return CONTINUE;
	}


	int _eventCount;
};





int start()
{
	X3DWriter* writer[2];
	writer[0] = new X3DWriterXML();
	writer[1] = new X3DWriterFI();

	for(int i = 0; i< 2; i++)
	{
		X3DWriter* w = writer[i];
		if (i == 0)
			w->OpenFile("iotest.x3d");
		else 
			w->OpenFile("iotest.x3db");
		w->StartDocument();
		w->StartNode(ID::X3D);
		w->StartNode(ID::Scene);
		w->StartNode(ID::Shape);
		w->StartNode(ID::Appearance);
		w->StartNode(ID::Material);
		w->SetSFVec3f(ID::diffuseColor, 1.0f, 0.0f, 0.0f);
		w->SetSFFloat(ID::transparency, 0.1f);
		w->EndNode();
		w->EndNode(); // Appearance
		w->StartNode(ID::Box);
		w->SetSFVec3f(ID::size, 0.5f, 0.5f, 0.5f);
		w->EndNode(); // Box
		w->EndNode();//Shape
		w->EndNode(); // Scene
		w->EndNode();
		w->EndDocument();
		w->CloseFile();

		delete w;
	}

	
	for(int i = 0; i< 2; i++)
	{
		X3DLoader loader;
		MyNodeHandler handler;
		loader.setNodeHandler(&handler);
		try {
			if (i == 0)
				loader.load("iotest.x3d");
			else
				loader.load("iotest.x3db");
		} catch (X3DParseException& e)
		{	
		  cerr << "Error while parsing file:" << endl;
		  cerr << e.getMessage() << " (Line: " << e.getLineNumber() << ", Column: " << e.getColumnNumber() << ")" << endl;
		  return 1;
		}
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

  //ah.new_string("input_filename.vtk", "The name of the input file", input_filename);
  //ah.new_string("output_filename", "The name of the output file", output_filename);
  
  //ARGUMENT_HELPER_BASICS(ah);
  ah.set_description("An I/O test application for the X3DLoader");
  ah.set_author("Kristian Sons, kristian.sons@actor3d.com");
  ah.set_version(0.9f);
  ah.set_build_date(__DATE__);

  ah.process(argc, argv);


  // Check output string
  return start();
  
  cerr << "Input file not found or not readable: " << input_filename << endl;
  return 1;
}