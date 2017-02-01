// Grasp.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include <io.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Interface to the Oculus and Windows
#include "../OculusInterface/OculusInterface.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

// Telemetry and photo services provided by DEX.
#include "../DexServices/DexServices.h"

#include "../GraspVR/GraspVR.h"
#include "GraspTaskManagers.h"

using namespace PsyPhy;
using namespace Grasp;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Hardware Interface
//

// I don't remember why these are here as static global variables, instead of as 
// static instance variables of the GraspVR class, but I don't want to spend time figuring it out.

// Interface to OpenGL windows and HMD.
static OculusDisplayOGL _oculusDisplay;

// Mapping and rendering in Oculus.
static OculusMapper _oculusMapper;

/*****************************************************************************/

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	
	bool useCoda = true;
	bool useHMD = true;
	enum { doVtoV, doVtoK, doVtoVK, doKtoK, doVtoVtraining, doVtoKtraining, doKtoKtraining, doDemo, doDoff } paradigm = doVtoV;

	char sequence_filename[FILENAME_MAX] = "";
	char output_filename_root[FILENAME_MAX] = "";
	char dex_log_filename[FILENAME_MAX] = "";

	// Parse the command line.
	int items;
	char *ptr;
	fOutputDebugString( "Grasp Command Line: %s\n", lpCmdLine );

	if ( strstr( lpCmdLine, "--nocoda" ) ) useCoda = false;
	if ( strstr( lpCmdLine, "--nohmd" ) ) useHMD = false;

	if ( ptr = strstr( lpCmdLine, "--sequence" ) ) items = sscanf( ptr, "--sequence=%s", sequence_filename );
	fAbortMessageOnCondition( (items == 0), "Grasp", "Error parsing command line argument.\n\n  %s\n\n(Remember: no spaces around '=')", ptr );

	if ( ptr = strstr( lpCmdLine, "--output" ) ) items = sscanf( ptr, "--output=%s", output_filename_root );
	fAbortMessageOnCondition( (items == 0), "Grasp", "Error parsing command line argument.\n\n  %s\n\n(Remember: no spaces around '=')", ptr );

	HWND parentWindow = nullptr;
	if ( ptr = strstr( lpCmdLine, "--parent=" ) ) sscanf( ptr, "--parent=%d", &parentWindow );

	//
	// Connect to dex for telemetry and snapshots.
	//
	DexServices *dex = new DexServices();
	dex->ParseCommandLine( lpCmdLine );
	strcpy( dex_log_filename, output_filename_root );
	strcat( dex_log_filename, ".dxl" );
	dex->Initialize( dex_log_filename );
	dex->Connect();
	dex->SendSubstep( 0 );
	dex->SnapPicture( "STARTUP" );

	//
	// Start up the VR display. 
	//
	GraspDisplay *display;
	if ( useHMD) {
		GraspOculusDisplay *oculus_display = new GraspOculusDisplay();
		oculus_display->Initialize( hInstance, &_oculusDisplay, &_oculusMapper, parentWindow );
		display = oculus_display;
	}
	else {
		GraspWindowsDisplay *windows_display = new GraspWindowsDisplay();
		windows_display->Initialize( hInstance, parentWindow );
		display = windows_display;
	}

	// 
	// Select a set of trackers and start them up.
	//
	GraspTrackers *trackers;
	CodaRTnetTracker *codaTracker;
	if ( useCoda ) {
	    codaTracker = new CodaRTnetDaemonTracker();
		trackers = new GraspOculusCodaTrackers( &_oculusMapper, codaTracker );
	}
	else if ( useHMD ) {
		trackers = new GraspOculusOnlyTrackers( &_oculusMapper );
	}
	else {
		trackers = new GraspSimulatedTrackers();
	}
	trackers->Initialize();

	// 
	// Select the paradigm and create the corresponding object to run the paradigm.
	//
	GraspTaskManager	*grasp;

	if ( strstr( lpCmdLine, "--VtoV" ) ) grasp = new VtoV();
	else if ( strstr( lpCmdLine, "--VtoVK" ) ) grasp = new VtoVK();
	else if ( strstr( lpCmdLine, "--VtoK" ) ) grasp = new VtoK();
	else if ( strstr( lpCmdLine, "--KtoK" ) ) grasp = new KtoK();
	else if ( strstr( lpCmdLine, "--demo" ) ) grasp = new DemoP();
	else if ( strstr( lpCmdLine, "--doff" ) ) grasp = new QuitVR();
	else grasp = new DemoP();

	
	//
	// Perform some hacks to handle some specific cases.
	//

	// During training, timeout for tilting the head is set very long, but the 
	// instructor can trigger the next phase (response phase) with a key press.
	if ( strstr( lpCmdLine, "--training" ) ) grasp->tiltHeadTimeout = 30.0;

	// Select the method for guiding the subject to the initial head position.
	// By default, the subject is simply asked to straighten the head on the shoulders and then press a button.
	// But the presence of these cookie files allows one to choose another behavior.
	// The 'Coda' method asks the subject to align the head to look down the Z axis of the Coda reference frame.
	if ( 0 == _access_s( "CodaAlignGaze.flg", 0x00 ) ) grasp->straightenHeadMethod = CODA_STRAIGHTEN;
	// The 'Chest' methods asks the subject to gaze in a direction defined by the hmd and chest marker arrays.
	if ( 0 == _access_s( "ChestAlignGaze.flg", 0x00 ) ) grasp->straightenHeadMethod = CHEST_STRAIGHTEN;

	grasp->Initialize( display, trackers, dex );
	int return_code = grasp->RunTrialBlock( sequence_filename, output_filename_root );
	grasp->Release();

	trackers->Release();
	if ( useCoda ) codaTracker->Quit();

	// Ask DEX to take a final snapshot and then disconnect.
	dex->SnapPicture( "RELEASE" );
	dex->ResetTaskInfo(); // It might be better to remove this and leave it up to the caller.
	dex->Disconnect();
	dex->Release();

	exit( return_code );

}


