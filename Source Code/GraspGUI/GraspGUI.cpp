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

	// Create the main window and run it.
	GraspDesktop^ gui = gcnew GraspDesktop();
	Application::Run( gui );

	return 0;
}



