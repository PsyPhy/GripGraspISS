// GraspTrackerDaemon.cpp : main project file.

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include <winsock2.h>
#include <io.h>


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
GraspTrackerRecord echo;


int main( int argc, char *argv[] )
{

	bool use_coda = true;
	bool confirm = false;
	for ( int arg = 1; arg < argc; arg++ ) {
		if ( !strcmp( argv[arg], "--nocoda" ) ) use_coda = false;
		if ( !strcmp( argv[arg], "--confirm" ) ) confirm = true;
	}
	if ( 0 == _access_s( "NoCoda.flg", 0x00 ) ) use_coda = false;

	// Start up the CODA.
	if ( use_coda ) {
		fprintf( stderr, "Initializing CODA ..." );
		codaTracker.Initialize();
		fprintf( stderr, "OK.\n" );
		for ( int unit = 0; unit < codaTracker.nUnits; unit++ ) {
			codaTracker.GetAlignment( record.alignmentOffset, record.alignmentRotation );
		}
	}
	else {
		fprintf( stderr, "Simulating CODA startup ..." );
		codaTracker.nUnits = MAX_UNITS;
		codaTracker.nMarkers = MAX_MARKERS;
		for ( int unit = 0; unit < codaTracker.nUnits; unit++ ) {
			codaTracker.CopyVector( record.alignmentOffset[unit], codaTracker.zeroVector );
			codaTracker.CopyMatrix( record.alignmentRotation[unit], codaTracker.identityMatrix );
		}
		Sleep( 5000 );
		fprintf( stderr, "OK.\n" );
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

	//struct sockaddr_in Receiver_addr;
	//Receiver_addr.sin_family = AF_INET;
	//Receiver_addr.sin_port = htons( TRACKER_DAEMON_PORT );
	//Receiver_addr.sin_addr.s_addr = INADDR_ANY;
	//enabled = TRUE;

	//struct sockaddr_in si_other;
	//int len_si_other = sizeof( si_other );

	//if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&enabled, sizeof(BOOL)) < 0 ) 
	//{
	//	closesocket(sock);
	//	fAbortMessage( "GraspTrackerDaemon", "Error setting socket options." );		
	//}

	//if ( bind( sock, (sockaddr*)&Receiver_addr, sizeof(Receiver_addr)) < 0)
	//{
	//	int error_value = WSAGetLastError();
	//	closesocket(sock);
	//	fAbortMessage( "GraspTrackerDaemon", "Error binding socket (%d).", error_value );		
	//}
	//unsigned long noBlock = 1;
	//ioctlsocket( sock, FIONBIO, &noBlock );

	while ( 1 ) {

		record.count = 0;
		while(1)
		{
			if ( use_coda ) {
				record.nUnits = codaTracker.nUnits;
				for ( int unit = 0; unit < record.nUnits; unit++ ) {
					codaTracker.GetCurrentMarkerFrameUnit( record.frame[unit], unit );
				}
			}
			else {
				record.count++;
				record.nUnits = codaTracker.nUnits;
				for ( int unit = 0; unit < record.nUnits; unit++ ) {
					record.frame[unit].time = (double) record.count / 200.0;
					for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
						if ( ( (mrk + record.count / 100 ) / 8 ) % 4 == unit ) record.frame[unit].marker[mrk].visibility = true;
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
				closesocket( sock );
				fAbortMessage( "GraspTrackerDaemon", "Error on sendto (%d).", error_value );		
			}
			fprintf( stderr, "message sent successfully\n" );
			//int recv_len = recvfrom( sock, (char *) &echo, sizeof( echo ), 0, (struct sockaddr *) &si_other, &len_si_other );
			//if ( recv_len != sizeof( record ) ) break;

			record.count++;
			Sleep( 1 );

			if ( _kbhit() ) {
				// Clear the keyboard hit.
				_getch();
				// Close the socket and clean up.
				closesocket(sock);
				WSACleanup();
				fprintf( stderr, "\nGraspTrackerDaemon terminated.\n" );
				if ( confirm ) {
					fprintf( stderr, "Press <Return> to close window.\n" );
					_getch();
				}
				exit( 0 );
			}

		}

		// Stop CODA acquisitions.
		if ( use_coda ) {
			fprintf( stderr, "Resetting CODA ... " );
			codaTracker.AbortAcquisition();
			codaTracker.Quit();
			fprintf( stderr, "OK.\n" );
		}
		else {
			fprintf( stderr, "Pausing to simulate CODA reset ... " );
			Sleep( 5000 );
			fprintf( stderr, "OK.\n" );
		}

	}

	return 0;

}
