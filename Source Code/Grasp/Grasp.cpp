// Grasp.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
//// Interface to the Oculus and Windows
//#include "../Trackers/CodaRTnetTracker.h"
//#include "../OculusInterface/OculusViewpoint.h"
//#include "../GraspVR/GraspGLObjects.h"
//#include "../GraspVR/GraspVR.h"


#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Interface to the Oculus and Windows
#include "../OculusInterface/OculusInterface.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

#include "../GraspVR/GraspVR.h"

using namespace PsyPhy;
using namespace Grasp;

/*****************************************************************************/
// A device that records 3D marker positions.
// Those marker positions will also drive the 6dof pose trackers.
PsyPhy::CodaRTnetTracker codaTracker;

#define MAX_CODA_MARKERS 28
int nMarkers = 24;
#define MAX_CODA_UNITS	8
int nCodaUnits = 2;

// Polling the CODA in the rendering loop can cause non-smooth updating.
// Here we use a thread to get the CODA pose data in the background.

bool lockCoda[MAX_CODA_UNITS] = { false, false, false, false, false, false, false, false };
bool stopMarkerGrabs = false;
MarkerFrame codaBuffer[MAX_CODA_UNITS];
HANDLE threadHandle;

void GetMarkerFrameFromBackground( int unit, MarkerFrame *destination ) {
	while ( lockCoda[unit] );
	lockCoda[unit] = true;
	memcpy( destination, &codaBuffer[unit], sizeof( *destination ) );
	lockCoda[unit] = false;
}

DWORD WINAPI GetCodaMarkerFramesInBackground( LPVOID prm ) {

	PsyPhy::CodaRTnetTracker *coda = (PsyPhy::CodaRTnetTracker *)prm;
	MarkerFrame localFrame;

	while ( !stopMarkerGrabs ) {
		for ( int unit = 0; unit < nCodaUnits; unit++ ) {
			coda->GetCurrentMarkerFrameUnit( localFrame, unit );
			while ( lockCoda[unit] );
			lockCoda[unit] = true;
			memcpy( &codaBuffer[unit], &localFrame, sizeof( codaBuffer[unit] ) );
			lockCoda[unit] = false;
		}
	}
	OutputDebugString( "GetCodaMarkerFramesInBackground() thread exiting.\n" );
	return NULL;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	bool useCoda = false;
	GraspVR	*vr;

	if ( useCoda ) {

		// Initialize the connection to the CODA tracking system.
		codaTracker.Initialize();

		// Start continuous acquisition of Coda marker data for a maximum duration.
		codaTracker.StartAcquisition( 600.0 );

		// Initiate real-time retrieval of CODA marker frames in a background thread 
		// so that waiting for the frame to come back from the CODA does not slow down
		// the rendering loop.
		DWORD threadID;
		stopMarkerGrabs = false;
		threadHandle = CreateThread( NULL, 0, GetCodaMarkerFramesInBackground, &codaTracker, 0, &threadID );

		// Choose a GraspVR combination for V-V using the DEX trackers.
		vr = new GraspVV();
	}

	else vr = new GraspSIM();

	vr->Initialize( hInstance );
	vr->DebugLoop();
	vr->Release();

	if ( useCoda ) {

		// Halt the Coda real-time frame acquisition that is occuring in a background thread.
		stopMarkerGrabs = true;
		WaitForSingleObject( threadHandle, INFINITE );
		// Halt the continuous Coda acquisition.
		codaTracker.StopAcquisition();

		// Output the CODA data to a file.
		char *filename = "Log\\PsyPhyOculusDemo.mrk";
		fOutputDebugString( "Writing CODA data to %s.\n", filename );
		FILE *fp = fopen( filename, "w" );
		if ( !fp ) fMessageBox( MB_OK, "File Error", "Error opening %s for write.", filename );

		fprintf( fp, "%s\n", filename );
		fprintf( fp, "Tracker Units: %d\n", codaTracker.GetNumberOfUnits() );
		fprintf( fp, "Frame\tTime" );
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
			for ( int unit = 0; unit < codaTracker.GetNumberOfUnits(); unit++ ) {
				fprintf( fp, "\tM%02d.%1d.V\tM%02d.%1d.X\tM%02d.%1d.Y\tM%02d.%1d.Z", mrk, unit, mrk, unit, mrk, unit, mrk, unit  );
			}
		}
		fprintf( fp, "\n" );

		for ( int frm = 0; frm < codaTracker.nFrames; frm++ ) {
			fprintf( fp, "%05d %9.3f", frm, codaTracker.recordedMarkerFrames[0][frm].time );
			for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
				for ( int unit = 0; unit < codaTracker.GetNumberOfUnits(); unit++ ) {
					fprintf( fp, " %1d",  codaTracker.recordedMarkerFrames[unit][frm].marker[mrk].visibility );
					for ( int i = 0; i < 3; i++ ) fprintf( fp, " %9.3f",  codaTracker.recordedMarkerFrames[unit][frm].marker[mrk].position[i] );
				}
			}
			fprintf( fp, "\n" );
		}
		fclose( fp );
		fOutputDebugString( "File %s closed.\n", filename );
	}
}


