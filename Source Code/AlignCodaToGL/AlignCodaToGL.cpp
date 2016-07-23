// AlignCodaToGL.cpp : Defines the entry point for the console application.
//

#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// RTNet C++ includes
#define NO64BIT
#include "../Include/codaRTNetProtocolCPP/RTNetClient.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsAlignment.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsCodaMode.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsCodaPacketMode.h"
#include "../Include/codaRTNetProtocolCPP/DeviceInfoAlignment.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecode3DResult.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecode3DResultExt.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecodeADC16.h"
#include "../Include/codaRTNetProtocolCPP/DeviceInfoUnitCoordSystem.h"
#include "../Trackers/CodaRTnetTracker.h"

#define DEVICEID_ADC DEVICEID_GS16AIO

using namespace codaRTNet;
using namespace PsyPhy;

char *serverAddress = "10.80.12.103";
unsigned int serverPort = 10111;

// The CODA system can really handle up to 8 units so create my static 
//  variables in preparation for this. Other code will use the #MAX_UNITS
//  that may be set lower than 8 to reflect the real max number of units
//  that we can expect to find.
#define REAL_MAX_UNITS 8
double offset[REAL_MAX_UNITS][3] = {
	{360.0, 0.0, 0.0},
	{360.0, 0.0, 0.0},
	{360.0, 0.0, 0.0},
	{360.0, 0.0, 0.0},
	{360.0, 0.0, 0.0},
	{360.0, 0.0, 0.0},
	{360.0, 0.0, 0.0},
	{360.0, 0.0, 0.0},
};

double rotation[REAL_MAX_UNITS][3][3] = {
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
	{{ -1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
};

int main(int argc, char *argv[])
{

	// Create and start up the CODA tracker.
	fprintf( stderr, "Starting up CODA ... ");
	CodaRTnetTracker *coda = new CodaRTnetTracker();
	coda->Initialize();
	fprintf( stderr, "OK.\n" );
	fprintf( stderr, "Sending CodaToGL alignment ... ");
	coda->SetAlignment( offset, rotation, "Results\\CodaToGL.alignment.dat" );
	fprintf( stderr, "OK.\n" );
	fprintf( stderr, "Shutting down CODA ... ");
	coda->Quit();
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "\nPress <Return> to exit.  " );
	_getch();

	return 0;
}
