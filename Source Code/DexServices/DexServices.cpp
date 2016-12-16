	// Module: DexServices
// This is the main DLL file.

#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "../Trackers/CodaRTnetNullTracker.h"
#include "../Trackers/PoseTrackers.h"
#include "TMdata.h"
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

	if ( filename ) strncpy( log_filename, filename, sizeof( log_filename ));
	log = fopen( log_filename, "a+" );
	fAbortMessageOnCondition( !log, "DexServices", "Error opening %s for write.", log_filename );
	printDateTime( log );
	fprintf( log, " File %s open for logging services.\n", log_filename );
}

int DexServices::Connect ( void ) {

	printDateTime( log );
	fprintf( log, " Attempting connection to DEX.\n" );

	// default to localhost
	const char *server_name= DEFAULT_SERVER;
	unsigned short port = DEFAULT_PORT;
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

	{ // Convert nnn.nnn address to a usable one
		addr = inet_addr(server_name);

		if(addr==INADDR_NONE)
		{
			fOutputDebugString( "DexServices: problem interpreting IP address.\n");
			WSACleanup();
			return( -2 );
		}
	}

	// Copy the resolved information into the sockaddr_in structure

	memset(&server, 0, sizeof(server));
	memcpy(&(server.sin_addr), &addr, 4);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	dexSocket = socket(AF_INET, socket_type, 0); /* Open a socket */

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
	printDateTime( log );
	fprintf( log, " Connection to DEX pending.\n" );
	Sleep( 1000 );
	return 0;

};

void DexServices::Disconnect( void ) {
	// If the connection failed, this should fail, too, but I don't really care.
	closesocket( dexSocket );
	WSACleanup();
	printDateTime( log );
	fprintf( log, " Connection closed.\n" );
}

void DexServices::Release( void ) {
	printDateTime( log );
	fprintf( log, " Closing log file.\n" );
	fclose( log );
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

void DexServices::AddDataSlice( unsigned int objectStateBits, PsyPhy::TrackerPose &hmd, PsyPhy::TrackerPose &codaHmd, PsyPhy::TrackerPose &hand, PsyPhy::TrackerPose &chest, PsyPhy::TrackerPose &mouse, MarkerFrame frame[2] ) {

	// We need a Pose Tracker and a Tracker just to get access to their internal copy functions.
	// I declare them static here so that they don't explode the stack.
	static PsyPhy::PoseTracker		pm;
	static CodaRTnetNullTracker		tr;
	// Fill the current slice with the new data.
	rt.Slice[slice_count].fillTime = (float) TimerElapsedTime( stream_timer );
	rt.Slice[slice_count].globalCount = stream_count++;
	rt.Slice[slice_count].objectStateBits = objectStateBits;
	// There is a bug in the following. The hmd pose is sent in the place of the hand, chest and mouse.
	// This needs to be fixed, but I am leaving it like this for now to be consistent with the flight software.
	pm.CopyTrackerPose( rt.Slice[slice_count].hmd, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].codaHmd, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].hand, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].chest, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].mouse, hmd );
	tr.CopyMarkerFrame( rt.Slice[slice_count].markerFrame[0], frame[0] );
	tr.CopyMarkerFrame( rt.Slice[slice_count].markerFrame[1], frame[1] );

	// Dex RT Packets can only be sent two times per second. 
	// So we pack multiple data slices into a single packet.
	// We set a minimum time between slices and if the caller 
	// sends a new slice before the timer runs out, we overwrite
	// the previous slice. When the timer does run out we increment
	// to the next slice. When there are enough slices to fill the
	// packet, it gets sent.
	if ( TimerTimeout( slice_timer ) ) {
		slice_count++;
		if ( slice_count >= GRASP_RT_SLICES_PER_PACKET ) {
			SendScienceRealtimeData();
			slice_count = 0;
		}
		TimerSet( slice_timer, GRASP_RT_SLICE_INTERVAL );
	}
}

int DexServices::SendScienceRealtimeData( void ) {

	// A buffer to hold the string of bytes that form the packet.
	u8 packet[1024];

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

int DexServices::SendTaskInfo( int user, int protocol, int task, int step, unsigned short substep, unsigned short tracker_status ) {

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
	
	if ( TimerTimeout( info_timer ) ) {
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
		TimerSet( info_timer, GRASP_HK_PACKET_INTERVAL );
		return( sent );

	}
	else return( 0 );

}

// Send a number to indicate the tracker status.
int DexServices::SendTrackerStatus( unsigned int status ) {
	return( SendTaskInfo( static_user, static_protocol, static_task, static_step, static_substep, status ) ); 
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

	if ( log ) {
		printDateTime( log );
		fprintf( log, " Snapped picture: %s (%s)\n", cam.tag, ( sent > 0 ? "OK" : "not sent") );
		fflush( log );
	}
	return( sent );
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

