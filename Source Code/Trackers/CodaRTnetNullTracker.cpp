/***************************************************************************/
/*                                                                         */
/*                               CodaRTnetTracker                          */
/*                                                                         */
/***************************************************************************/

#include "stdafx.h"

// A tracker that uses the CODA RTnet interface.

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/ini.h"

#include "CodaRTnetNullTracker.h"

using namespace PsyPhy;

void CodaRTnetNullTracker::Initialize( const char *ini_filename ) {
	acquiring = false;
	overrun = false;
	nUnits = 2;
	nMarkers = 24;
	// Fill the last frame with a record in which all the markers are invisible.
	// This will be sent as the current frame until such time that a real frame has been read.
	unsigned int index = 0;
	for ( int unit = 0; unit < nUnits; unit++ ) {
		nFramesPerUnit[unit] = 1;
		recordedMarkerFrames[unit][index].time = 0.0;
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) recordedMarkerFrames[unit][index].marker[mrk].visibility = false;
	}
	nFrames = 1;
	Sleep( 3000 );
}

void CodaRTnetNullTracker::StartContinuousAcquisition( void ) {}

void CodaRTnetNullTracker::StartAcquisition( double duration ) {
	TimerSet( acquisitionTimer, duration );
	for ( int unit = 0; unit < nUnits; unit++ ) {
		nFramesPerUnit[unit] = 1;
		recordedMarkerFrames[unit][0].time = TimerElapsedTime( acquisitionTimer );
	}
	nFrames = 1;
	overrun = false;
	acquiring = true;
	StartContinuousAcquisition();
}

void CodaRTnetNullTracker::StopAcquisition( void ) {
	acquiring = false;
}
void CodaRTnetNullTracker::AbortAcquisition( void ) {
	acquiring = false;
	nFrames = 1;
}
void CodaRTnetNullTracker::Quit( void ) {}

bool CodaRTnetNullTracker::GetAcquisitionState( void ) {
	Update();
	return( acquiring );
}

int CodaRTnetNullTracker::Update( void ) {
	if ( acquiring ) {
		for ( int unit = 0; unit < nUnits; unit++ ) {
			recordedMarkerFrames[unit][nFramesPerUnit[unit]].time = TimerElapsedTime( acquisitionTimer );
			if ( nFramesPerUnit[unit] < MAX_FRAMES - 1 ) nFramesPerUnit[unit]++;
		}
	}
	return( true );
}

bool CodaRTnetNullTracker::GetCurrentMarkerFrameUnit( MarkerFrame &frame, int selected_unit ) {
	// Make sure that any packets that were sent were read.
	Update();
	CopyMarkerFrame( frame, recordedMarkerFrames[selected_unit][nFramesPerUnit[selected_unit] - 1] );
	return true;
}

void CodaRTnetNullTracker::GetAlignment( Vector3 offset[MAX_UNITS], Matrix3x3 rotation[MAX_UNITS] ) {
	for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
		CopyVector( offset[unit], zeroVector );
		CopyMatrix( rotation[unit], identityMatrix );
	}
}


