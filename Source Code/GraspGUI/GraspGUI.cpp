// GraspGUI.cpp : main project file.

#include "stdafx.h"
#include "GraspDesktopForm.h"

using namespace GraspGUI;

[STAThreadAttribute]
int main( array<System::String ^> ^args )
{

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window.
	GraspDesktop^ gui = gcnew GraspDesktop();

	// The path to the results directory can be overidden with a command line argument.
	String^ results_path;
	if ( args->Length > 0 ) results_path = args[0];
	else results_path = gcnew String( "Results\\" );
	gui->SetResultsPath( results_path );

	// Run it.
	Application::Run( gui );

	return 0;
}



