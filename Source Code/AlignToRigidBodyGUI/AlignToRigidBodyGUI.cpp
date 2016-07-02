// AlignToRigidBodyGUI.cpp : main project file.

#include "stdafx.h"

#include "../DexServices/DexServices.h"
#include "SingleObjectAlignForm.h"
#include "MultiObjectAlignForm.h"

using namespace AlignToRigidBodyGUI;
using namespace Grasp;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	String ^filename_root = args->Length >= 2 ? args[1] : gcnew String( "CodaAlignment" );
	String ^model_file = args->Length >= 1 ? args[0] : gcnew String( "Bdy\\Chest.bdy" );

	// Establish a connection with DEX for transmitting housekeeping and marker visibility.
	//DexServices *dex = new DexServices();
	//dex->Connect();

	// Create the main window and run it
	Application::Run(gcnew SingleObjectForm( model_file, filename_root ));
	return( Environment::ExitCode );

}
