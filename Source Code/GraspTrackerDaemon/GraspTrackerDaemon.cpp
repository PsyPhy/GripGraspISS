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

#include "GraspTrackerDaemon.h"

using namespace PsyPhy;

// A device that records 3D marker positions.
CodaRTnetContinuousTracker codaTracker;

// A structured data buffer with fields for all of the tracker data.
GraspTrackerRecord record;
int record_length = sizeof( record );


int main( int argc, char *argv[] )
{

	bool use_coda = true;
	bool confirm = false;
	for ( int arg = 1; arg < argc; arg++ ) {
		if ( !strcmp( argv[arg], "--nocoda" ) ) use_coda = false;
		if ( !strcmp( argv[arg], "--confirm" ) ) confirm = true;
	}

	// Start up the CODA.
	if ( use_coda ) {
		fprintf( stderr, "Initializing CODA ..." );
		codaTracker.Initialize();
		fprintf( stderr, "OK.\n" );
		fprintf( stderr, "Starting acquisition ... " );
		codaTracker.StartAcquisition();
		fprintf( stderr, "OK.\n\n" );
	}
	else {
		codaTracker.nUnits = 0;
		codaTracker.nMarkers = 0;
	}

	// Initialize a socket to which we will broadcast the data.
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) fAbortMessage( "GraspTrackerDaemon", "Error creating socket." );

	BOOL enabled = TRUE;
	if ( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(BOOL)) < 0 ) 
	{
		closesocket(sock);
		fAbortMessage( "GraspTrackerDaemon", "Error setting broadcast options." );		
	}

	struct sockaddr_in Sender_addr;
	Sender_addr.sin_family = AF_INET;
	Sender_addr.sin_port = htons( TRACKER_DAEMON_PORT );
	Sender_addr.sin_addr.s_addr = inet_addr( TRACKER_BROADCAST_ADDRESS );

	record.count = 0;
	while(1)
	{
		if ( use_coda ) {
			record.nUnits =  codaTracker.nUnits;
			for ( int unit = 0; unit < record.nUnits; unit++ ) {
				codaTracker.GetCurrentMarkerFrameUnit( record.frame[unit], unit );
			}
		}
		else {
			record.nUnits = 2;
			record.count++;
			for ( int unit = 0; unit < record.nUnits; unit++ ) {
				record.frame[unit].time = (double) record.count / 200.0;
				for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					if ( ( mrk / 8 ) == unit ) record.frame[unit].marker[mrk].visibility = true;
					else record.frame[unit].marker[mrk].visibility = false;
				}
			}
		}
		for ( int unit = 0; unit < record.nUnits; unit++ ) {
			fprintf( stderr, "U%d: %6.3f ", unit, record.frame[unit].time );
			for ( int mrk = 0; mrk < 24; mrk++ ) {
				if ( record.frame[unit].marker[mrk].visibility ) fprintf( stderr, "O" );
				else fprintf( stderr, "." );
				if ( ((mrk+1) % 8) == 0 ) fprintf( stderr, "   " );
			}
			fprintf( stderr, "  " );
		}

		if ( sendto(sock, (char *) &record, sizeof( record ), 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
		{
			int error_value = WSAGetLastError();
			closesocket(sock);
			fAbortMessage( "GraspTrackerDaemon", "Error on sendto (%d).", error_value );		
		}

		fprintf( stderr, "message sent successfully\n" );

		record.count++;
		Sleep( 1 );

		if ( _kbhit() ) break;

	}

	// Clear the keyboard hit.
	_getch();
	// Close the socket and clean up.
	closesocket(sock);
	WSACleanup();
	// Stop CODA acquisitions.
	if ( use_coda ) codaTracker.AbortAcquisition();

	fprintf( stderr, "\nGraspTrackerDaemon terminated.\n" );
	if ( confirm ) {
		fprintf( stderr, "Press <Return> to close window.\n" );
		_getch();
	}

	return 0;

}
