// GraspTrackerDaemon.cpp : main project file.

#include "stdafx.h"
#include "GraspTrackerDaemonForm.h"

using namespace GraspTrackerDaemon;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Form1^ form = gcnew Form1();
	for ( int i = 0; i < args->Length; i++ ) {
		if ( args[i]->StartsWith( "--hide" ) ) form->autohide = true;
	}
	Application::Run( form );
	return 0;
}
	