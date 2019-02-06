// Module: DexServices
// This is the main DLL file.

#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <assert.h>


#include "../Trackers/CodaRTnetNullTracker.h"
#include "../Trackers/PoseTrackers.h"

#include "DexServices.h"

#include "../Useful/fOutputDebugString.h"
#include "../Useful/fMessageBox.h"


using namespace Grasp;
using namespace PsyPhy;

void DexServices::printDateTime( FILE *fp ) {
	SYSTEMTIME st;
	GetSystemTime( &st );
	fprintf( fp, "%4d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
}

void DexServices::Initialize( char *filename ) {

	if ( filename ) {
		log = fopen( filename, "a+" );
		fAbortMessageOnCondition( !log, "DexServices", "Error opening %s for write.", filename );
		printDateTime( log );
		fprintf( log, " File %s open for logging services.\n", filename );
	}
	else log = NULL;

	// Clear the realtime science data buffer.
	for ( int i = 0; i < GRASP_RT_SLICES_PER_PACKET; i++ ) {
		rt.Slice[i].fillTime = 0.0;
		rt.Slice[i].globalCount = 0;
		CopyTrackerPose( rt.Slice[i].hmd, NullTrackerPose );
		CopyTrackerPose( rt.Slice[i].hand, NullTrackerPose );
		CopyTrackerPose( rt.Slice[i].chest, NullTrackerPose );
		rt.Slice[i].clientTime = 0.0;
		strncpy(  (char *) rt.Slice[i].clientData, "NULL", sizeof( rt.Slice[i].clientData ) );
	}

}

int DexServices::Connect ( void ) {

	if ( log ) {
		printDateTime( log );
		fprintf( log, " Attempting connection to DEX.\n" );
	}

	int retval;
	unsigned long addr;
	int socket_type = SOCK_STREAM;
	struct sockaddr_in server;
	WSADATA wsaData;

	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
		fOutputDebugString( "DexServices: WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return -1;
	}
	else
	{
		fOutputDebugString( "DexServices: WSAStartup() OK.\n");
	}

	{ 
		// Convert nnn.nnn address to a usable one
		addr = inet_addr( server_name );
		if(addr==INADDR_NONE)
		{
			fOutputDebugString( "DexServices: problem interpreting IP address: %s.\n", server_name );
			WSACleanup();
			return( -2 );
		}
	}

	// Copy the resolved information into the sockaddr_in structure

	memset( &server, 0, sizeof(server) );
	memcpy( &(server.sin_addr), &addr, 4 );
	server.sin_family = AF_INET;
	server.sin_port = htons( server_port );

	dexSocket = socket( AF_INET, socket_type, 0 ); /* Open a socket */

	if ( dexSocket < 0 )
	{
		fOutputDebugString( "DexServices: Error Opening socket: Error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	fOutputDebugString( "Client: socket() OK.\n" );
	// Set TCP_NODELAY to reduce jitter
	unsigned int flag = 1;
	setsockopt( dexSocket,	/* socket affected */
		IPPROTO_TCP,		/* set option at TCP level */
		TCP_NODELAY,		/* name of option */
		(char *) &flag,					
		sizeof( flag ));	/* length of option value */

	fOutputDebugString( "DexServices: Client connecting to: %s.\n", server_name );

	// Try to connect to the host, but without blocking.
	// If we are not connected to DEX we can still work.
	unsigned long nonblocking = 1;
	ioctlsocket( dexSocket, FIONBIO, &nonblocking );
	connect( dexSocket, (struct sockaddr*)&server, sizeof(server) );
	if ( log ) {
		printDateTime( log );
		fprintf( log, " Connection to DEX pending.\n" );
	}

	TimerSet( slice_timer, GRASP_RT_SLICE_INTERVAL );
	TimerSet( housekeeping_timer, GRASP_HK_PACKET_INTERVAL );
	TimerStart( stream_timer );

	return 0;

};

void DexServices::Disconnect( void ) {
	fOutputDebugString( "DexServices: Disconnecting from server.\n" );
	// If the connection failed, this should fail, too, but I don't really care.
	closesocket( dexSocket );
	if ( log ) {
		printDateTime( log );
		fprintf( log, " Connection closed.\n" );
	}
	fOutputDebugString( "DexServices: Connection to server disconnected.\n" );
}

void DexServices::Release( void ) {
	if ( log ) {
		printDateTime( log );
		fprintf( log, " Closing log file.\n" );
		fclose( log );
	}
	WSACleanup();
	fOutputDebugString( "DexServices: released.\n" );
}

int DexServices::Send( const unsigned char *packet, int size ) {
	// Attempt to send a packet. If the connection has not been established, this will fail, but we carry on.
	// The caller may wish to signal the error or not, depending on the return code.
	int retval = send( dexSocket, (const char *) packet, size, 0);
	// If we were fancier, we could check if the connection has been achieved before doing the send and depending
	// on the error we could try to reconnnect. But instead I just assume that if the connection was not established
	// by the first try, we just carry on without a connection.
	if ( retval == SOCKET_ERROR && not_sent_countdown > 0 ) {
		fOutputDebugString( "DexServices: send() failed: error %d (may just be waiting for connection.)\n", WSAGetLastError());
		not_sent_countdown--;
	}
	return retval;
}

void DexServices::NullifyClientInfo( void ) {
		for ( int i = 0; i < GRASP_RT_SLICES_PER_PACKET; i++ ) {
			strncpy(  (char *) rt.Slice[i].clientData, "NULL", sizeof( rt.Slice[i].clientData ) );
		}
}

void DexServices::AddTrackerSlice( PsyPhy::TrackerPose &hmd, PsyPhy::TrackerPose &hand, PsyPhy::TrackerPose &chest, MarkerFrame frame[MAX_UNITS] ) {

	// Fill the current slice with the new data.
	rt.Slice[slice_count].fillTime = (float) TimerElapsedTime( stream_timer );
	rt.Slice[slice_count].globalCount = stream_count++;
	// Copy the current poses to the slice.
	CopyTrackerPose( rt.Slice[slice_count].hmd, hmd );
	CopyTrackerPose( rt.Slice[slice_count].hand, hand );
	CopyTrackerPose( rt.Slice[slice_count].chest, chest );
	// And the marker information (3D position and visibility).
	for ( int unit = 0; unit < MAX_UNITS; unit++ ) CopyMarkerFrame( rt.Slice[slice_count].markerFrame[unit], frame[unit] );

	AdvanceIfReady();

}

void DexServices::AddClientSlice( unsigned char *data, int bytes, bool advance ) {

	int max_bytes = sizeof( rt.Slice[slice_count].clientData );
	assert( bytes <= max_bytes );
	rt.Slice[slice_count].clientTime = (float) TimerElapsedTime( stream_timer );
	// Copy what we have been given into the slice.
	memcpy( rt.Slice[slice_count].clientData, data, bytes );
	// Set the rest of the client data to zero.
	for ( int i = bytes; i < max_bytes; i++  ) rt.Slice[slice_count].clientData[i] = 0;

	if ( advance ) Advance();
	else AdvanceIfReady();

}

void DexServices::Advance( void ) {
		slice_count++;
		if ( slice_count >= GRASP_RT_SLICES_PER_PACKET ) {
			SendScienceRealtimeData();
			NullifyClientInfo();
			slice_count = 0;
		}
		TimerSet( slice_timer, GRASP_RT_SLICE_INTERVAL );
}

void DexServices::AdvanceIfReady( void ) {

	// Dex RT Packets can only be sent two times per second. 
	// So we pack multiple data slices into a single packet.
	// We set a minimum time between slices and if the caller 
	// sends a new slice before the timer runs out, we overwrite
	// the previous slice. When the timer does run out we increment
	// to the next slice. When there are enough slices to fill the
	// packet, it gets sent, and we reset back to the begining of the
	// slice buffer.
	if ( TimerTimeout( slice_timer ) ) Advance();

}

int DexServices::SendScienceRealtimeData( void ) {

	// fOutputDebugString( "DexServices: SendScienceRealtimeData()\n" );

	// A buffer to hold the string of bytes that form the packet.
	u8 packet[10240];

	// Turn the data structure into a string of bytes with header etc.
	u32 size = rt.serialize( packet );

	// Send it to DEX.
	int sent = Send( packet, size );
	if ( log ) {
		printDateTime( log );
		fprintf( log, " Sent science realtime info (%s).\n", ( sent > 0 ? "OK" : "not sent") );
		fflush( log );
	}
	return( sent );

}

int DexServices::SendTaskInfo( int user, int protocol, int task, int step, unsigned short substep, unsigned short tracker_status, bool force ) {

	// A buffer to hold the string of bytes that form the packet.
	u8 packet[1024];
	// An object that serializes the data destined for DEX housekeeping telemetry packets.
	HK_packet hk;

	// Save the current values as a convenience so that one can call SendSubstep() later on.
	static_user = user;
	static_protocol = protocol;
	static_task = task;
	static_step = step;
	static_substep = substep;
	static_tracker_status = tracker_status;

	if ( TimerTimeout( housekeeping_timer ) ) {

		// Fill the packet with info.
		hk.current_protocol = protocol;
		hk.current_user = user;
		hk.current_task = task;
		hk.current_step = step;
		hk.scriptengine_status = substep;
		hk.motiontracker_status = tracker_status;

		// Turn the data structure into a string of bytes with header etc.
		u32 size = hk.serialize( packet );

		// Send it to DEX.
		int sent = Send( packet, size );
		if ( log ) {
			printDateTime( log );
			fprintf( log, " Sent task info: %d %d %d %d   %d %d (%s).\n", 
				user, protocol, task, step, substep, tracker_status, ( sent > 0 ? "OK" : "not sent") );
			fflush( log );
		}
		TimerSet( housekeeping_timer, GRASP_HK_PACKET_INTERVAL );
		return( sent );

	}
	else return( 0 );

}

int DexServices::ResendTaskInfo( void ) { 
	return( SendTaskInfo( static_user, static_protocol, static_task, static_step, static_substep, static_tracker_status, true ) ); 
}

int DexServices::SendSubstep( int substep ) { 
	return( SendTaskInfo( static_user, static_protocol, static_task, static_step, STEP_EXECUTING + ( substep % 10000 ), static_tracker_status ) ); 
}

// Send a number to indicate the tracker status.
int DexServices::SendTrackerStatus( unsigned int status ) {
	return( SendTaskInfo( static_user, static_protocol, static_task, static_step, static_substep, status ) ); 
}

void DexServices::ParseCommandLine( char *command_line ) {
	char *ptr;
	int items= -1;
	if ( ptr = strstr( command_line, "--user" ) ) {
		items = sscanf( ptr, "--user=%d", &static_user );
		// If items == 1 then we successufully parsed the item as well.
		// But if items = 0, it means that we recognized a --key but that the reading of the value was not successful.
		fAbortMessageOnCondition( (items == 0), "DexServices", "Error parsing command line argument.\n\n  %s\n\n(Remember: no spaces around '=')", ptr );
	}
	if ( ptr = strstr( command_line, "--protocol" ) ) {
		items = sscanf( ptr, "--protocol=%d", &static_protocol );
		fAbortMessageOnCondition( (items == 0), "DexServices", "Error parsing command line argument.\n\n  %s\n\n(Remember: no spaces around '=')", ptr );
	}
	if ( ptr = strstr( command_line, "--task" ) ) {
		items = sscanf( ptr, "--task=%d", &static_task );
		// If items == 1 then we successufully parsed the item as well.
		// But if items = 0, it means that we recognized a --key but that the reading of the value was not successful.
		fAbortMessageOnCondition( (items == 0), "DexServices", "Error parsing command line argument.\n\n  %s\n\n(Remember: no spaces around '=')", ptr );
	}
	if ( ptr = strstr( command_line, "--step" ) ) {
		items = sscanf( ptr, "--step=%d", &static_step );
		// If items == 1 then we successufully parsed the item as well.
		// But if items = 0, it means that we recognized a --key but that the reading of the value was not successful.
		fAbortMessageOnCondition( (items == 0), "DexServices", "Error parsing command line argument.\n\n  %s\n\n(Remember: no spaces around '=')", ptr );
	}
	if ( ptr = strstr( command_line, "--dexlog" ) ) {
		char argument[256];
		items = sscanf( ptr, "--dexlog=%d", argument );
		sprintf( log_filename, "%s.dxl" );

		// If items == 1 then we successufully parsed the item as well.
		// But if items = 0, it means that we recognized a --key but that the reading of the value was not successful.
		fAbortMessageOnCondition( (items == 0), "DexServices", "Error parsing command line argument.\n\n  %s\n\n(Remember: no spaces around '=')", ptr );
	}

}

int DexServices::SnapPicture( const char *tag ) {

	// A buffer to hold the string of bytes that form the packet.
	u8 packet[1024];
	// An object that serializes the data destined for DEX housekeeping telemetry packets.
	CameraTrigger_packet cam;

	// Truncate tags that are too long.
	if ( strlen( tag ) < sizeof( cam.tag ) ) strcpy( cam.tag, tag );
	else {
		// Take only the part that fits.
		memcpy( cam.tag, tag, sizeof( cam.tag ) );
		// Make sure that it is null terminated.
		cam.tag[ sizeof(cam.tag) - 1 ] = '\0';
		// Emit a warning.
		fOutputDebugString( "DexServices: Warning - Truncating tag %s to %s\n", tag, cam.tag );
	}

	// Turn the data structure into a string of bytes with header etc.
	u32 size = cam.serialize( packet );

	// Send it to DEX.
	int sent = Send( packet, size );
	fOutputDebugString( " Snapped picture: %s (%s)\n", cam.tag, ( sent > 0 ? "OK" : "not sent") );

	if ( log ) {
		printDateTime( log );
		fprintf( log, " Snapped picture: %s (%s)\n", cam.tag, ( sent > 0 ? "OK" : "not sent") );
		fflush( log );
	}
	return( sent );
}


void DexServices::InitializeProxySocket( void ) {

	WSADATA wsaData;
	int iResult;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if ( iResult != 0 ) {
		fOutputDebugString( "WSAStartup failed with error: %d\n", iResult );
		return;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo( NULL, PROXY_DEX_PORT_STRING, &hints, &result );
	if ( iResult != 0 ) {
		fOutputDebugString("getaddrinfo() failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to client
	proxySocket = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
	if ( proxySocket == INVALID_SOCKET ) {
		fOutputDebugString( "socket() failed with error: %ld\n", WSAGetLastError() );
		freeaddrinfo( result );
		WSACleanup();
		return;
	}
	
	// Make the receiver buffer bigger so that we don't miss any packets.
	int bufferSize = 100 * sizeof( DexServicesPacket );
	int bufferSizeLen = sizeof( bufferSize);
	setsockopt( proxySocket, SOL_SOCKET, SO_RCVBUF, (char *) &bufferSize, bufferSizeLen);


	// Setup the TCP listening socket
	iResult = bind( proxySocket, result->ai_addr, (int)result->ai_addrlen );
	if (iResult == SOCKET_ERROR) {
		fOutputDebugString("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo( result );
		closesocket( proxySocket );
		WSACleanup();
		return;
	}

	unsigned long nonblocking = 1;
	ioctlsocket( proxySocket, FIONBIO, &nonblocking );
	// Set to listen.
	fOutputDebugString( "Listening for a client connection on port %s.\n", PROXY_DEX_PORT_STRING  );
	iResult = listen( proxySocket, SOMAXCONN );
	if (iResult == SOCKET_ERROR) {
		fOutputDebugString("listen() failed with error: %d\n", WSAGetLastError());
		closesocket( proxySocket );
		WSACleanup();
		return;
	}

	// We don't need the address info anymore, so free it.
	freeaddrinfo(result);

}

bool DexServices::HandleProxyConnection( void ) {

	// If there is no connected client, the socket will be marked as INVALID. 
	if ( clientSocket == INVALID_SOCKET ) {
		// If not already connected, attempt to accept a new connect, but don't block.
		clientSocket = accept( proxySocket, NULL, NULL );
		int error_code;
		if ( clientSocket == INVALID_SOCKET ) {
			if ( WSAEWOULDBLOCK != ( error_code = WSAGetLastError() ) ) {
				// If we are here, there was an error other than that there is no pending connection.
				fOutputDebugString( "accept() failed with error: %d\n", error_code );
				closesocket( proxySocket );
				WSACleanup();
			}
			// Whether there was an error or just that there is no pending connection,
			// return false to indicate that we are not connected.
			return( false );
		}
		TimerSet( client_connection_timer, CLIENT_CONNECTION_TIMEOUT );
		// New connection, so initialize the client data ID.
		NullifyClientInfo();

	}

	// We are connected so attempt to read a packet.
	DexServicesPacket pk;
	int iResult;
	while ( ( iResult = recv( clientSocket, (char *) &pk, sizeof( DexServicesPacket ), 0) ) > 0 ) {

		// Process the packet.
		if ( iResult == sizeof( DexServicesPacket ) ) {
			fOutputDebugString( "DexServices::HandleProxyConnection() - Received Command: %d  Count: %d\n", pk.command, pk.sequence_number );
			switch ( pk.command ) {

			case HOUSEKEEPING:
				SendTaskInfo( pk.user, pk.protocol, pk.task, pk.step, pk.substep, pk.tracker );
				break;

			case SUBSTEP:
				SendSubstep( pk.substep );
				break;

			case TRACKER_STATUS:
				SendTrackerStatus( pk.tracker );
				break;

			case PICTURE:
				SnapPicture( pk.tag );
				break;

			case TRACKER_DATA:
				AddTrackerSlice( pk.hmd, pk.chest, pk.hand, pk.markers );
				break;

			case CLIENT_DATA:
				AddClientSlice( pk.client, sizeof( pk.client ) );
				break;

			case DISCONNECT:
				closesocket( clientSocket );
				clientSocket = INVALID_SOCKET;
				return( false );
				break;

			default:
				fOutputDebugString( "DexServices::HandleProxyConnection() - Unexpected command: %d\n", pk.command );

			}
		 
		}
		else {
			fOutputDebugString( "DexServices::HandleProxyConnection() - Unexpected packet size: %d\n", iResult );
		}

		// Reset the timeout timer because we received a packet.
		TimerSet( client_connection_timer, CLIENT_CONNECTION_TIMEOUT );
		// Let the caller know that we are connected.
		return( true );
	}

	if ( TimerTimeout( client_connection_timer ) ) {
		// We didn't get anything from the client for some time
		// check to see if the connection still alive by sending something.
		// What is sent is not actually read on the other side.
		char discard[8] = "DISCARD";
		iResult = send( clientSocket, discard, sizeof( discard ), 0 );
		if ( iResult < 0 ) {
			closesocket( clientSocket );
			clientSocket = INVALID_SOCKET;
			return( false );
		}
		else TimerSet( client_connection_timer, CLIENT_CONNECTION_TIMEOUT );
	}

	return( true );

}

void DexServices::ReleaseProxySocket( void ) {
	closesocket( proxySocket );
	closesocket( clientSocket );
	WSACleanup();
}


