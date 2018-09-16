// GraspMMIColumbus.cpp : main project file.

#include "stdafx.h"
#include "GraspMMIColumbusForm.h"

using namespace GraspMMIColumbus;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew GraspMMIColumbusForm());
	return 0;
}
