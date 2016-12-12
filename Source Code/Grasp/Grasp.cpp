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
	enum { doVtoV, doVtoK, doVtoVK, doKtoK, doVtoVtraining, doVtoKtraining, doKtoKtraining, doDemo, doDoff } paradigm = doVtoV;

	char sequence_filename[FILENAME_MAX];
	char output_filename_root[FILENAME_MAX];
	char dex_log_filename[FILENAME_MAX];

	// Parse the command line.
	int items;
	char *ptr;
	fOutputDebugString( "Grasp Command Line: %s\n", lpCmdLine );

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
	GraspDisplay *display = new GraspDisplay();
	display->Initialize( hInstance, &_oculusDisplay, &_oculusMapper, parentWindow );

	// 
	// Select a set of trackers and start them up.
	//
	GraspTrackers *trackers;
	if ( strstr( lpCmdLine, "--nocoda" ) ) useCoda = false;
	if ( useCoda ) {
		CodaRTnetTracker *codaTracker = new CodaRTnetDaemonTracker();
		trackers = new GraspOculusCodaTrackers( &_oculusMapper, codaTracker );
	}
	else trackers = new GraspSimTrackers( &_oculusMapper );
	trackers->Initialize();
	
	// 
	// Select the paradigm and create the corresponding object.
	//
	GraspTaskManager	*grasp;

	if ( strstr( lpCmdLine, "--VtoV" ) ) paradigm = doVtoV;
	if ( strstr( lpCmdLine, "--VtoVK" ) ) paradigm = doVtoVK;
	if ( strstr( lpCmdLine, "--VtoK" ) ) paradigm = doVtoK;
	if ( strstr( lpCmdLine, "--KtoK" ) ) paradigm = doKtoK;
	if ( strstr( lpCmdLine, "--demo" ) ) paradigm = doDemo;
	if ( strstr( lpCmdLine, "--doff" ) ) paradigm = doDoff;

	switch ( paradigm ) {

	case doVtoVK:
		grasp = new VtoVK();
		break;

	case doVtoK:
		grasp = new VtoK();
		break;

	case doKtoK:
		grasp = new KtoK();
		break;

	case doVtoV:
		grasp = new VtoV();
		break;

	case doDoff:
		grasp = new QuitVR();
		break;

	case doDemo:
	default:
		grasp = new DemoP();
		break;

	}

	//
	// Perform some hacks to handle some specific cases.
	//

	// During training, timeout for tilting the head is set very long.
	if ( strstr( lpCmdLine, "--training" ) ) grasp->tiltHeadTimeout = 30.0;

	// Select the method for guiding the subject to the initial head position.
	if ( 0 == _access_s( "CodaAlignGaze.flg", 0x00 ) ) grasp->straightenHeadMethod = CODA_STRAIGHTEN;
	if ( 0 == _access_s( "ChestAlignGaze.flg", 0x00 ) ) grasp->straightenHeadMethod = CHEST_STRAIGHTEN;

	grasp->Initialize( display, trackers, dex );
	int return_code = grasp->RunTrialBlock( sequence_filename, output_filename_root );
	grasp->Release();

	dex->SnapPicture( "RELEASE" );
	dex->ResetTaskInfo();
	dex->Disconnect();
	dex->Release();

	exit( return_code );

}


