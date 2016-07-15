// AlignToRigidBodyGUI.cpp : main project file.

#include "stdafx.h"

#include "../DexServices/DexServices.h"
#include "SingleObjectAlignForm.h"

using namespace AlignToRigidBodyGUI;
using namespace Grasp;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Parse the command line arguments.
	// Note that unrecognized arguments will be ignored here, but they might be recognized
	//  further down when DexServices parses the command line arguments as well.
	String ^model_file = gcnew String( "Bdy\\Chest.bdy" );
	String ^filename_root = gcnew String( "CodaAlignment" );
	for ( int i = 0; i < args->Length; i++ ) {
		if ( args[i]->StartsWith( "--body" ) ) model_file = args[i]->Substring( args[i]->IndexOf( '=' ) + 1 );
		if ( args[i]->StartsWith( "--output" ) )filename_root = args[i]->Substring( args[i]->IndexOf( '=' ) + 1 );
	}
	fOutputDebugString( "AlignToRigidBodyGUI - Model file: %s   Results Filename Root: %s\n", model_file, filename_root );

	// Establish a connection with DEX for transmitting housekeeping and marker visibility.
	DexServices *dex = new DexServices();
	dex->ParseCommandLineArguments( args );
	dex->Initialize();
	dex->Connect();
	dex->SendSubstep( 0 );

	// Create the main window and run it
	Application::Run(gcnew SingleObjectForm( model_file, filename_root, dex ));
	return( Environment::ExitCode );

}
