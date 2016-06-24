// Grasp.cpp : Defines the entry point for the application.
//
#include "stdafx.h"

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Interface to the Oculus and Windows
#include "../OculusInterface/OculusInterface.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

#include "../GraspVR/GraspVR.h"
#include "GraspTaskManagers.h"
#include "GraspTelemetry.h"

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
	
	bool useCoda = false;
	enum { doVtoV, doVtoK, doVtoVK, doKtoK, doVtoVtraining, doVtoKtraining, doKtoKtraining } paradigm = doVtoV;

	char sequence_filename[FILENAME_MAX];
	char output_filename_root[FILENAME_MAX];

	// Parse the command line.
	fOutputDebugString( "Grasp Command Line: %s\n", lpCmdLine );
	if ( strstr( lpCmdLine, "--nocoda" ) ) useCoda = false;
	if ( strstr( lpCmdLine, "--VtoV" ) ) paradigm = doVtoV;
	if ( strstr( lpCmdLine, "--VtoVK" ) ) paradigm = doVtoVK;
	if ( strstr( lpCmdLine, "--VtoK" ) ) paradigm = doVtoK;
	if ( strstr( lpCmdLine, "--KtoK" ) ) paradigm = doKtoK;
	if ( char *ptr = strstr( lpCmdLine, "--sequence=" ) ) sscanf( ptr, "--sequence=%s", sequence_filename );
	if ( char *ptr = strstr( lpCmdLine, "--output=" ) ) sscanf( ptr, "--output=%s", output_filename_root );

	GraspTaskManager	*grasp;
	GraspTrackers		*trackers;

	// ConnectToDex();

	if ( useCoda ) trackers = new GraspDexTrackers( &_oculusMapper );
	else trackers = new GraspSimTrackers( &_oculusMapper );
	
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
	default:
		grasp = new VtoV();
		break;

	}
	grasp->Initialize( hInstance, &_oculusDisplay, &_oculusMapper, trackers );
	grasp->RunTrialBlock( sequence_filename, output_filename_root );
	grasp->Release();
//	trackers->WriteDataFiles( output_filename_root );

	exit( 0 );

}


