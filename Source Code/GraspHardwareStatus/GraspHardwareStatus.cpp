// GraspHardwareStatus.cpp : main project file.

#include "stdafx.h"
#include "GraspHardwareStatusForm.h"
#include "../OculusInterface/CodaSimTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"

using namespace GraspHardwareStatus;
using namespace PsyPhy;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	Sleep( 100 );

	CodaRTnetDaemonTracker *tracker = new CodaRTnetDaemonTracker();
	tracker->Initialize();

	MarkerFrame codaFrame;

	CodaPoseTracker *hmdTracker = new CodaPoseTracker( &codaFrame );
	hmdTracker->ReadModelMarkerPositions( "Bdy\\HMD.bdy" );

	CodaPoseTracker *handTracker = new CodaPoseTracker( &codaFrame );
	handTracker->ReadModelMarkerPositions( "Bdy\\Hand.bdy" );

	CodaPoseTracker *chestTracker = new CodaPoseTracker( &codaFrame );
	chestTracker->ReadModelMarkerPositions( "Bdy\\Chest.bdy" );

	// Establish a connection with DEX for transmitting housekeeping and marker visibility.
	DexServices *dex = new DexServices();
	// See if the caller has specified the user, protocol, task and step numbers. We don't change those here.
	dex->ParseCommandLineArguments( args );
	dex->Initialize();

	// Create the main window and run it
	Application::Run(gcnew Form1( tracker, hmdTracker, handTracker, chestTracker ));

	tracker->Quit();

	return 0;
}
