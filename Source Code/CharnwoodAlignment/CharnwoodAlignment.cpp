// CharnwoodAlignment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>

#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include "../Useful/fMessageBox.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaPoseTracker.h"

using namespace PsyPhy;

// A device that records 3D marker positions that will drive the 6dof pose trackers.
// This has to be a static variable because otherwise it overflows the stack.
CodaRTnetTracker codaTracker;


int _tmain(int argc, _TCHAR* argv[])
{
	bool confirm = true;

	fprintf( stderr, "%s started.\n\n", argv[0] );

	// Initialize the connection to the CODA tracking system.
	fprintf( stderr, "Initializing CODA system ..." );
	codaTracker.Initialize();
	fprintf( stderr, "OK.\n" );

	codaTracker.PerformAlignment( 8, 9, 8, 8, 10, true );

	return 0;
}

