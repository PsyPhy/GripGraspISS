// GripAimingTool.cpp : main project file.

#include "stdafx.h"
#include "GripAimingForm.h"

using namespace GripAimingTool;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew GripAimingForm());
	return 0;
}
