// GraspHardwareStatus.cpp : main project file.

#include "StdAfx.h"

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

	Form1^	form = gcnew Form1();

	form->coda = new CodaRTnetDaemonTracker();
	form->coda->Initialize();

	MarkerFrame codaFrame;
	form->markerFrame = &codaFrame;

	form->hmdTracker = new CodaPoseTracker( &codaFrame );
	form->hmdTracker->ReadModelMarkerPositions( "Bdy\\HMD.bdy" );

	form->handTracker = new CodaPoseTracker( &codaFrame );
	form->handTracker->ReadModelMarkerPositions( "Bdy\\Hand.bdy" );

	form->chestTracker = new CodaPoseTracker( &codaFrame );
	form->chestTracker->ReadModelMarkerPositions( "Bdy\\Chest.bdy" );

	// Create a default output filename.
	String^ root = gcnew String( "GraspHardwareStatus" );

	// Parse for command line arguments.
	for ( int i = 0; i < args->Length; i++ ) {
		if ( args[i]->StartsWith( "--output" ) ) root = args[i]->Substring( args[i]->IndexOf( '=' ) + 1 );
	}

	// Create the main window and run it
	form->filenameRoot = root;
	Application::Run( form );

	form->coda->Quit();

	return 0;
}
