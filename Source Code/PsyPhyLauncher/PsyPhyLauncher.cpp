// PsyPhyLauncher.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace PsyPhyLauncher;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	Form1^ form = gcnew Form1();

	form->Size = System::Drawing::Size( 200, 100 * args->Length );

	// Create the main window and run it
	Application::Run( form );
	return 0;
}
