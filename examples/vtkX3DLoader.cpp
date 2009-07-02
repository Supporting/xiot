#include "Argument_helper.h"
#include <iostream>
#include <string>
#include <fstream>
#include "X3DLoader.h"
#include "X3DDefaultNodeHandler.h"
#include "X3DAttributes.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkVRMLImporter.h"
#include "vtkX3DImporter.h"
#include "vtkLight.h"
#include "vtkTimerLog.h"
#include "vtkVRMLExporter.h"

using namespace std;

#define PERFORMANCE_RUNS 15

string input_filename;
bool testPerformance = false;

int showScene(vtkImporter* importer)
{
	vtkRenderer *renderer = vtkRenderer::New();
	renderer->SetBackground(0.0, 0.0, 0.0);
	vtkLight* headLight = vtkLight::New();
	headLight->SetLightTypeToHeadlight();
	headLight->SetColor(1.0, 1.0, 1.0);
	headLight->SetIntensity(1.0);
	renderer->AddLight(headLight);
	headLight->Delete();
	renderer->SetAmbient(0.0, 0.0, 0.0);
	renderer->SetLightFollowCamera(1);
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(renderer);
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	vtkTimerLog* timer = vtkTimerLog::New();
	timer->StartTimer();

	importer->SetRenderWindow(renWin);
	importer->Read();
	timer->StopTimer();


	renWin->SetSize(300,300);
	renWin->Render();

	if (dsr::verbose)
		cout << "Time to load file: " << timer->GetElapsedTime() << endl;

	iren->Start();

	importer->Delete();
	renderer->Delete();
	renWin->Delete();
	iren->Delete();
	return 0;
}

int performTest(vtkImporter* importer)
{
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	importer->SetRenderWindow(renWin);
	importer->Read();
	renWin->Delete();
	importer->Delete();
	return 0;
}

int loadVRML(string input_filename)
{
	vtkVRMLImporter* importer = vtkVRMLImporter::New();
	importer->SetFileName(input_filename.c_str());
	importer->SetDebug(dsr::VERBOSE ? 1 : 0);
	return showScene(importer);
}

int loadX3D(string input_filename)
{
	vtkX3DImporter* importer = vtkX3DImporter::New();
	importer->SetFileName(input_filename.c_str());
	importer->SetDebug(dsr::VERBOSE ? 1 : 0);
	importer->SetVerbose(dsr::verbose ? 1 : 0);
	return showScene(importer);
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

	ah.new_string("input_filename", "The name of the input file. Can be either VRML or X3D.", input_filename);
	
	//ARGUMENT_HELPER_BASICS(ah);
	ah.set_description("A demonstrator that allows loading of VRML or X3D scenes. It demonstrates the capabilities of the generic X3D loader library.");
	ah.set_author("Kristian Sons, kristian.sons@actor3d.com");
	ah.set_version(0.9f);
	ah.set_build_date(__DATE__);

	ah.process(argc, argv);


	// Check output string
	if (fileExists(input_filename))
	{
		string extension = input_filename.substr(input_filename.find_last_of('.') + 1, input_filename.size());
		if (extension == "x3d" || extension == "x3db")
		{
			return loadX3D(input_filename);
		}
		else if (extension == "vrml" || extension == "wrl")
		{
			return loadVRML(input_filename);
		}

		cerr << "Can't detect filetype from extension: " << extension << endl;
		cerr << "Known extensions: wrl, vrml, x3d" << extension << endl;
		return 1;
	}

	cerr << "Input file not found or not readable: " << input_filename << endl;
	return 1;
}
