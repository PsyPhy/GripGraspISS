// WaitForCodaDaemon.cpp 
// A program that simply waits until it sees that the GraspTrackerDaemon is outputing data.
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include <WinSock2.h>
#include <windows.h>
#include <mmsystem.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetContinuousTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"

using namespace PsyPhy;
int main(int argc, char *argv[])
{
	fprintf( stderr, "Waiting for CodaRTnetDaemon ..." );
	PsyPhy::CodaRTnetDaemonTracker *codaTracker = new PsyPhy::CodaRTnetDaemonTracker();
	codaTracker->Initialize();
	codaTracker->Quit();
	return 0;
}

