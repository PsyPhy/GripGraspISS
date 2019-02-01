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

	HWND hwnd;
	hwnd = FindWindow( NULL, "c:\\windows\\system32\\cmd.exe" );
	// hwnd = FindWindow( NULL, "Command Prompt" );
	if ( hwnd ) ShowWindow( hwnd, SW_FORCEMINIMIZE );

	// Parse the command line arguments.
	// Note that unrecognized arguments will be ignored here, but they might be recognized
	//  further down when DexServices parses the command line arguments as well.
	String ^model_file = gcnew String( "Bdy\\Chest.bdy" );
	String ^filename_root = gcnew String( "CodaAlignment" );
	bool use_daemon = true;
	for ( int i = 0; i < args->Length; i++ ) {
		if ( args[i]->StartsWith( "--body" ) ) model_file = args[i]->Substring( args[i]->IndexOf( '=' ) + 1 );
		if ( args[i]->StartsWith( "--output" ) ) filename_root = args[i]->Substring( args[i]->IndexOf( '=' ) + 1 );
		if ( args[i]->StartsWith( "--direct" ) ) use_daemon = false;
	}

	// Establish a connection with DEX for transmitting housekeeping and marker visibility.
	DexServicesByProxy *dex = new DexServicesByProxy();
	// Create a name for the log file using the filename root defined above.
	char *log_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( filename_root + ".dxl" ).ToPointer();
	dex->Initialize( log_file );
	System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( log_file ) );
	dex->Connect();
	// See if the caller has specified the user, protocol, task and step numbers. We don't change those here.
	dex->ParseCommandLineArguments( args );
	// Initialize the sub-step to zero and send it to the ground via DEX. 
	// We will change this as the work progresses to indicate progress on this step.
	dex->SendSubstep( 0 );
	dex->SnapPicture( "PREALIGN" );

	// Create the CODA tracker.
	CodaRTnetTracker *coda;
	
	if ( use_daemon ) coda = new CodaRTnetDaemonTracker();
	else coda = new CodaRTnetTracker();

	// Create the main window and run it
	Application::Run(gcnew SingleObjectForm( model_file, filename_root, dex, coda ));

	// Ask DEX to take a final snapshot and then disconnect.
	dex->SnapPicture( "POSTALGN" );
	dex->Disconnect();
	dex->Release();

	return( Environment::ExitCode );



}
