// GraspTrackerDaemon.cpp : main project file.

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include<winsock2.h>

#include <windows.h>
#include <mmsystem.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"

#define BROADCAST_PORT		8888
#define BROADCAST_ADDRESS	"localhost"
using namespace PsyPhy;

// A device that records 3D marker positions.
CodaRTnetContinuousTracker codaTracker;

int main( int argc, char *argv[] )
{


	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) fAbortMessage( "GraspTrackerDaemon", "Error creating socket." );

	BOOL enabled = TRUE;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(BOOL)) < 0) 
	{
		closesocket(sock);
		fAbortMessage( "GraspTrackerDaemon", "Error setting broadcast options." );		
	}

	struct sockaddr_in Sender_addr;
	Sender_addr.sin_family = AF_INET;
	Sender_addr.sin_port = htons( BROADCAST_PORT );
	Sender_addr.sin_addr.s_addr = inet_addr( BROADCAST_ADDRESS );

	while(1)
	{

		Sleep(3000);
		if (sendto(sock, "Joe", strlen( "Joe" ) + 1, 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
		{
			closesocket(sock);
			fAbortMessage( "GraspTrackerDaemon", "Error on sendto." );		
		}

		fprintf( stderr, "message sent successfully\n" );

		if ( _kbhit() ) break;

	}

	_getch();
	closesocket(sock);

	WSACleanup();
	return 0;


	MarkerFrame localFrame[MAX_UNITS];

	fprintf( stderr, "Initializing CODA ..." );
	codaTracker.Initialize();
	fprintf( stderr, "OK.\n" );
	fprintf( stderr, "Starting acquisition ... " );
	codaTracker.StartAcquisition();
	fprintf( stderr, "OK.\n\n" );

	while ( _kbhit() == 0 ) {
		for ( int unit = 0; unit < codaTracker.nUnits; unit++ ) {
			codaTracker.GetCurrentMarkerFrameUnit( localFrame[unit], unit );
			fprintf( stderr, "U%d: %6.3f ", unit, localFrame[unit].time );
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				if ( localFrame[unit].marker[mrk].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				if ( ((mrk+1) % 8) == 0 ) fprintf( stderr, "   " );
			}
			fprintf( stderr, "  " );
		}
		fprintf( stderr, "\n" );
	}
	int key = _getch(); // Clear the _kbhit().
	codaTracker.AbortAcquisition();

	fprintf( stderr, "\nCodaVisibility terminated. Press <Return> to close window.\n" );
	_getch();

	return 0;

}
