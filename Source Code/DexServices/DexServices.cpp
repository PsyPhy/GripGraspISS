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

#include "../../../GripMMI/GripSourceCode/Grip/GripPackets.h"
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
	PsyPhy::PoseTracker pm;
	Tracker		tr;
	rt.Slice[slice_count].fillTime = TimerElapsedTime( stream_timer );
	rt.Slice[slice_count].globalCount = stream_count++;
	rt.Slice[slice_count].objectStateBits = objectStateBits;
	pm.CopyTrackerPose( rt.Slice[slice_count].hmd, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].codaHmd, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].hand, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].chest, hmd );
	pm.CopyTrackerPose( rt.Slice[slice_count].mouse, hmd );
	tr.CopyMarkerFrame( rt.Slice[slice_count].markerFrame[0], frame[0] );
	tr.CopyMarkerFrame( rt.Slice[slice_count].markerFrame[1], frame[1] );

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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// Read realtime GRASP science data packets into a local buffer.
///
int GetGraspRT( GraspRealtimeDataSlice grasp_data_slice[], int max_slices, char *filename_root ) {

	// Keep track of the last packet TM counter from previous call.
	// This is how we know if new data has arrived.
	static unsigned short previousTMCounter = 0;

	// Keep track of whether we have already seen that the buffers are full.
	static BOOL buffers_full_alert = FALSE;

	// Buffers and structures to hold data from the real time science packets.
	EPMTelemetryPacket		packet;
	EPMTelemetryHeaderInfo	epmHeader;
	GripRealtimeDataInfo	rt;

	// Will hold the filename (path) of the packet file.
	char filename[MAX_PATHLENGTH];
	// Will hold the pointer to the open packet file.
	int  fid;

	// Various local counters and flags.
	int bytes_read;
	int packets_read;
	int return_code;

	double previous_packet_timestamp;
	int n_slices;

	// Create the path to the realtime science packet file, based on the root and the packet type.
	// The global variable 'packetBufferPathRoot' has been initialized elsewhere.
	CreateGripPacketCacheFilename( filename, sizeof( filename ), GRIP_RT_SCIENCE_PACKET, filename_root );	

	// Attempt to open the packet cache to read the accumulated packets.
	// If it is not immediately available, keep trying for a few seconds.
	for ( int retry_count = 0; retry_count  < MAX_OPEN_CACHE_RETRIES; retry_count ++ ) {
		// Try to open the packet cache file.
		fid = _sopen( filename, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IWRITE | _S_IREAD  );
		// If open succeeds, it will return zero. So if zero return, break from retry loop.
		if ( fid >= 0 ) break;
		// Wait a second before trying again.
		Sleep( RETRY_PAUSE );
	}
	// If fid is negative, file is not open. This should not happen, because GripMMIStartup should verify 
	// the availability of files containing packets before the GripMMIDesktop form is executed.
	// But if we do fail to open the file, just signal the error and exit the hard way.
	if ( fid < 0 ) {
			fMessageBox( MB_OK, "GripMMI", "Error opening packet file %s.\n\n", filename );
			exit( -1 );
	}

	// Prepare for reading in packets. This is used to calculate the elapsed time between two packets.
	// By setting it to zero here, the first packet read will be signaled as having arrived after a long delay.
	previous_packet_timestamp = 0.0;

	// Read in all of the data packets in the file.
	// Be careful not to overrun the data buffers.
	packets_read = 0;
	n_slices = 0;
	while ( n_slices < max_slices ) {

		// Attempt to read next packet. Any error is terminal.
		bytes_read = _read( fid, &packet, rtPacketLengthInBytes );
		if ( bytes_read < 0 ) {
			fMessageBox( MB_OK, "GripMMI", "Error reading from %s.\n\n", filename );
			exit( -1 );
		}

		// If the number of bytes read is less than the expected number
		//  we are at the end of the file and should break out of the loop.
		if ( rtPacketLengthInBytes != bytes_read ) break;

		// We have a valid packet.
		packets_read++;

		// Check that it is a valid GRIP packet. It would be strange if it was not.
		ExtractEPMTelemetryHeaderInfo( &epmHeader, &packet );
		if ( epmHeader.epmSyncMarker != EPM_TELEMETRY_SYNC_VALUE || epmHeader.TMIdentifier != GRIP_RT_ID ) {
			fMessageBox( MB_OK, "GripMMIlite", "Unrecognized packet from %s.\n\n%s", filename );
			exit( -1 );
		}
			
		// Packets are stings of bytes. Extract the data values into a more usable form.
		ExtractGripRealtimeDataInfo( &rt, &packet );

		// If there has been a break in the arrival of the packets, insert
		//  a blank frame into the data buffer. This will cause breaks in
		//  the traces in the data graphs.
		if ( (rt.packetTimestamp - previous_packet_timestamp) > PACKET_STREAM_BREAK_THRESHOLD ) {
			// Subsampling in graphs will be used when the data record is very long.
			// Insert enough points so that we see the break even if we are sub-sampling in the graphs.
			// MAX_PLOT_STEP defines the maximum number of frames that will be skipped when plotting.
			for ( int count = 0; count < PACKET_STREAM_BREAK_INSERT_SAMPLES && n_slices < max_slices - 1; count++ ) {
				grasp_data_slice[n_slices].timestamp = MISSING_DOUBLE;
				n_slices++;
			}
		}
		previous_packet_timestamp = rt.packetTimestamp;

		// Each packet is a set of slices. Extract each one.
		for ( int count = 0; count < GRASP_RT_SLICES_PER_PACKET && n_slices < max_slices; count++ ) {
			// Get the time of the slice.
			grasp_data_slice[n_slices].timestamp = 0.0;
			// Count the number of slices.
			n_slices++;
		}

	}
	// Finished reading. Close the file and check for errors.
	return_code = _close( fid );
	if ( return_code ) {
		fMessageBox( MB_OK, "GripMMI", "Error closing %s after binary read.\nError code: %s\n\n", filename, return_code );
		exit( return_code );
	}
	fOutputDebugString( "Acquired Data Slices (max %d): %d\n", max_slices, n_slices );
	if ( n_slices >= max_slices ) {
		char filename2[MAX_PATHLENGTH]; 
		CreateGripPacketCacheFilename( filename2, sizeof( filename ), GRIP_HK_BULK_PACKET, filename_root );
		fMessageBox( MB_OK | MB_ICONERROR, "DexServices", 
			"Internal buffers are full.\n\nYou can continue plotting existing data.\nTracking of script progress will also continue.\n\nTo resume following new data transmissions:\n\n1) Halt all processes.\n2) Rename or move:\n      %s\n      %s\n3) Restart using RunGraspMMI.bat.",
			filename, filename2 );
		// Signal for the next call that we have already reached the limit of the buffers.
		buffers_full_alert = true;
	}
	
	return ( n_slices );
}