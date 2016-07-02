// HaltCoda.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
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

#define DEVICEID_ADC DEVICEID_GS16AIO

using namespace codaRTNet;

char *serverAddress = "10.80.12.103";
unsigned int serverPort = 10111;

int _tmain(int argc, _TCHAR* argv[])
{

	// client connection object
	codaRTNet::RTNetClient cl;

	try {
		
		// Decode the IP address string that has already been initialized as part of the class.
		unsigned int p, q, r, s;
		fOutputDebugString( "HaltCoda: %s   Port: %d  Configuration: %d\n", serverAddress, serverPort );
		sscanf( serverAddress, "%d.%d.%d.%d", &p, &q, &r, &s );
		
		// Connect to the server.
		cl.connect( (p << 24) + (q << 16) + (r << 8) + s, serverPort );
		cl.stopAcq();
		fOutputDebugString( "Shutting down ... " );
		cl.stopSystem();
		fOutputDebugString( "OK.\n" );
	}

	catch(NetworkException& exNet)
	{
		fOutputDebugString( "Caught (NetworkException& exNet)\n" );
		exit( -1 );
	}
	catch(DeviceStatusArray& array)
	{
		fOutputDebugString( "Caught (DeviceStatusArray& array)\n" );
		exit( -1 );
	}

	return 0;
}

