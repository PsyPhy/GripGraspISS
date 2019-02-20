// GripAimingTool.cpp : main project file.

#include "stdafx.h"

#include "GripAimingForm.h"

using namespace GripAimingTool;

[STAThreadAttribute]

int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	// Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	CodaRTnetDaemonTracker *tracker = new CodaRTnetDaemonTracker();
	tracker->Initialize( "CodaRTnet.ini" );

	// Create the main window and run it
	GripAimingForm^ form = gcnew GripAimingForm( tracker );
	Application::Run( form );

	return 0;
}
