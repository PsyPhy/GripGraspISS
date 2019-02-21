// GripAimingTool.cpp : main project file.

#include "stdafx.h"
#include "../DexServices/DexServices.h"
#include "GripAimingForm.h"

using namespace GripAimingTool;

[STAThreadAttribute]

int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	// Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Establish a connection with DEX for transmitting housekeeping and marker visibility.
	DexServicesByProxy *dex = new DexServicesByProxy();
	// Do not log anything.
	dex->Initialize( nullptr );
	dex->Connect();
	// See if the caller has specified the user, protocol, task and step numbers. We don't change those here.
	dex->ParseCommandLineArguments( args );
	// Initialize the sub-step to zero and send it to the ground via DEX. 
	// We will change this as the work progresses to indicate progress on this step.
	dex->SendSubstep( 0 );
	dex->SnapPicture( "PREAIM" );


	CodaRTnetDaemonTracker *tracker = new CodaRTnetDaemonTracker();
	tracker->Initialize( "EpmRTnet.ini" );

	// Create the main window and run it
	GripAimingForm^ form = gcnew GripAimingForm( tracker );
	Application::Run( form );

	tracker->Quit();

	// Ask DEX to take a final snapshot and then disconnect.
	dex->SnapPicture( "POSTAIM" );
	dex->ResetTaskInfo();
	dex->Disconnect();
	dex->Release();


	return 0;
}
