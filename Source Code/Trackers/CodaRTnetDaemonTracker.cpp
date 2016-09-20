/***************************************************************************/
/*                                                                         */
/*                           CodaRTnetDaemonTracker                        */
/*                                                                         */
/***************************************************************************/

#include "stdafx.h"

// A tracker that uses the CODA RTnet interface via the server daemon.

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/ini.h"

#include "../GraspTrackerDaemon/GraspTrackerDaemon.h"
#include "CodaRTnetDaemonTracker.h"

#define UDP_BUFFER_SIZE_IN_PACKETS 32

using namespace PsyPhy;

/*********************************************************************************/

void CodaRTnetDaemonTracker::StartAcquisition( double duration ) {
	overrun = false;
	acquiring = true;
	TimerSet( timer, duration );
	// Start placing data at the beginning of the buffer.
	nFrames = 0;
}

void CodaRTnetDaemonTracker::StopAcquisition( void ) {
	// Attempt to halt an ongoing aquisition. 
	// Does not care if it was actually acquiring or not.
	// Does not retrieve the data.
	OutputDebugString( "Stopping acquisition ..." );
	acquiring = false;
	OutputDebugString( "OK.\n" );
}

bool CodaRTnetDaemonTracker::GetAcquisitionState( void ) {
	Update();
	return( acquiring );
}

void CodaRTnetDaemonTracker::AbortAcquisition( void ) {
	// Attempt to halt an ongoing aquisition. 
	// Does not care if it was actually acquiring or not.
	// Does not retrieve the data.
	OutputDebugString( "Aborting acquisition ..." );
	acquiring = false;
	// By setting nFrames to 0, we effectively discard the data.
	nFrames = 0;
	OutputDebugString( "OK.\n" );
}

int CodaRTnetDaemonTracker::Update( void ) {
	
	// This tracker basically just catches and stores data from a daemon server that does the actual talking to CODA.
	// In the update handler we process any new available packets from the daemon. Note that it is entirely possible
	// that we are not catching every frame, especially if we don't call Update() often enough.

	GraspTrackerRecord record;

    // Clear any pending inputs and only take the last one.
	static int cycle_counter = 0;
	int packet_count;
	int recv_len, nError;
    for ( packet_count = 0; true; packet_count++ ) {
		recv_len = recvfrom( daemonSocket, (char *) &record, sizeof( record ), 0, (struct sockaddr *) &daemonAddr, &daemonAddrLength);
		nError = WSAGetLastError();
		// We are using non-blocking calls so that if there is no data we continue on with other things.
		// It is normal to receive WSAEWOULDBLOCK when no data is available on the socket.
		if ( nError == WSAEWOULDBLOCK ) break;
		// Treat all other errors rather dramatically by aborting.
		if ( nError != 0 ) fAbortMessage( "CodaRTnetDaemonTracker", "recvfrom() failed with error code : %d" , nError );
		// Here I check if the record is the right size. From what I have gotten off of the internet, this should
		// should always be true. But I thought I would check anyway.
		if ( recv_len != sizeof( record ) ) fAbortMessage( "TestTrackerDaemon", "recvfrom() returned unexpected byte count: %d vs. %d" , recv_len, sizeof( record ) );
		// Set the number of units to the number of units send by the daemon. This should not change in real time,
		// but this sets the number of units correcty when the first packet is read and processed here.
		nUnits = record.nUnits;
		for ( int unit = 0; unit < nUnits; unit++ ) CopyMarkerFrame( recordedMarkerFrames[unit][nFrames % MAX_FRAMES], record.frame[unit] );
		// If the time limit has expired, we stop acquiring a time series of frames.
		if ( TimerTimeout( timer ) ) acquiring = false;

		// If we are acquiring, move on to the next frame. Note the % (remainder) in the previous lines. We implement
		//  a circular buffer that never overflows, but that can lose early data points.
		if ( acquiring ) nFrames++;
	}
	if ( packet_count >= UDP_BUFFER_SIZE_IN_PACKETS ) fOutputDebugString( "Daemon Tracker Update: %d %d\n", cycle_counter, packet_count );
	cycle_counter++;
	fOutputDebugString( "nUnits %d recv_len %d  nError  %d  Packet count: %d\n", nUnits, recv_len, nError, packet_count );
	return( true );
	
}

bool CodaRTnetDaemonTracker::GetCurrentMarkerFrameUnit( MarkerFrame &frame, int selected_unit ) {
	// Make sure that any packets that were sent have been read.
	Update();
	if ( acquiring ) CopyMarkerFrame( frame, recordedMarkerFrames[selected_unit][nFrames - 1] );
	else CopyMarkerFrame( frame, recordedMarkerFrames[selected_unit][nFrames] );
	return true;
}

void CodaRTnetDaemonTracker::Initialize( const char *ini_filename ) {
	
	// If there is an .ini file, parse it for serial numbers, server address, etc.
	// As of now, this server does not use any of those parameters, but it might in the future.
	if ( ini_filename ) {
		fOutputDebugString( "CodaRTnetDaemonTracker: Parsing %s.\n", ini_filename );
		ini_parse( ini_filename, iniHandler, this );
	}

	if ( daemonSocket != NULL && daemonSocket != INVALID_SOCKET ) closesocket( daemonSocket );
		
	daemonAddrLength = sizeof( daemonAddr );

	// Create and prepare the socket used to receive the UDP datagrams from the daemon.
	// Make sure that the GraspTrackerDaemon has time to bind its socket.
	WSADATA wsaData;
	WSAStartup( MAKEWORD(2, 2), &wsaData );

	Sleep( 500 );
	daemonSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (daemonSocket == INVALID_SOCKET) fAbortMessage( "GraspTrackerDaemon", "Error creating socket." );

	// Set SO_REUSEADDR for the socket so that other processes can listen to the same data stream.
	bool enabled = true;
	if ( setsockopt( daemonSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &enabled, sizeof(enabled)) < 0 ) 
	{
		closesocket( daemonSocket );
		daemonSocket = NULL;
		fAbortMessage( "CodaRTnetDaemonTracker", "Error setting broadcast options." );		
	}

	// Make the receiver buffer bigger so that we don't miss any packets.
	int bufferSize = UDP_BUFFER_SIZE_IN_PACKETS * sizeof( GraspTrackerRecord );
	int bufferSizeLen = sizeof(bufferSize);
	setsockopt( daemonSocket, SOL_SOCKET, SO_RCVBUF, (char *) &bufferSize, bufferSizeLen);

	// Listen for datagrams from the specified address.
	struct sockaddr_in Recv_addr;
	Recv_addr.sin_family = AF_INET;
	Recv_addr.sin_port = htons( TRACKER_DAEMON_PORT );
	Recv_addr.sin_addr.s_addr = inet_addr( TRACKER_BROADCAST_ADDRESS );
	
	int bind_value;
	if ( (bind_value = bind( daemonSocket, (sockaddr*) &Recv_addr, sizeof( Recv_addr )) ) == SOCKET_ERROR )
	{
		int error_value = WSAGetLastError();
		closesocket( daemonSocket );
		daemonSocket = NULL;
		fAbortMessage( "CodaRTnetDaemonTracker", "Error binding socket (%d %d).", bind_value, error_value  );		
	}
	// Set the socket to do nonblocking calls to allow performing other actions while waiting for new data.
	unsigned long noBlock = 1;
	ioctlsocket( daemonSocket, FIONBIO, &noBlock );

	// Initialize the state of the acqusition.
	nFrames = 0;
	acquiring = false;
	nUnits = 0;
	Timer timer;
	// Wait until we get at least one frame from the daemon. It will set the true number of units.
	TimerSet( timer, 15.0 );
	while ( nUnits == 0 ) {
		if ( TimerTimeout( timer ) ) fAbortMessage( "CodaRTnetDaemonTracker", "Timeout waiting for tracker daemon." );
		Update();
		Sleep( 100 );
	}
}
void CodaRTnetDaemonTracker::Quit(void ) {
	closesocket( daemonSocket );
	daemonSocket = NULL;
	fOutputDebugString( "Quitting CodaRTnetTracker but leaving GraspTrackerDaemon runnning ..." );
}


void CodaRTnetDaemonTracker::Shutdown( void ) {
	// Need to quit to close the socket connection.
	Quit();
	// Now shutdown the daemon.
	system( "TaskKill /IM GraspTrackerDaemon.exe" );
	Sleep( 2000 );
}
void CodaRTnetDaemonTracker::Startup( void ) {
	system( "Start \"GRASP Tracker\" /MIN Executables\\GraspTrackerDaemon.exe" );
	// Re-establish a connection with the daemon.
	Initialize();
}
