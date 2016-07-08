// Module: DexServices
// This is the main DLL file.

#include <winsock2.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "../Useful/fOutputDebugString.h"
#include "../Useful/fMessageBox.h"

#include "TMData.h"
#include "DexServices.h"

using namespace Grasp;

void DexServices::printDateTime( FILE *fp ) {
	SYSTEMTIME st;
	GetSystemTime( &st );
	fprintf( fp, "%4d-%02d-%02d %02d:%02d:%02d.%3d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
}


void DexServices::Initialize( const char *filename ) {
	 log = fopen( filename, "w" );
	 fAbortMessageOnCondition( !log, "DexServices", "Error opening %s for write.", filename );
	 printDateTime( log );
	 fprintf( log, " File %s open for logging services.\n", filename );
}

int DexServices::Connect ( void ) {

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
		fOutputDebugString( "Client: WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return -1;
	}
	else
	{
		fOutputDebugString( "Client: WSAStartup() OK.\n");
	}

	{ // Convert nnn.nnn address to a usable one
		addr = inet_addr(server_name);

		if(addr==INADDR_NONE)
		{
			fOutputDebugString( "Client: problem interpreting IP address.\n");
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
		fOutputDebugString( "Client: Error Opening socket: Error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else
	{
		fOutputDebugString( "Client: socket() OK.\n" );
	}
	// Set TCP_NODELAY to reduce jitter
	unsigned int flag = 1;
	setsockopt( dexSocket,	/* socket affected */
		IPPROTO_TCP,		/* set option at TCP level */
		TCP_NODELAY,		/* name of option */
		(char *) &flag,					
		sizeof( flag ));	/* length of option value */

	fOutputDebugString( "Client: Client connecting to: %s.\n", server_name );

	if (connect( dexSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)

	{
		fOutputDebugString( "Client: connect() failed: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else
	{
		fOutputDebugString( "ConnectToDex() OK.\n" );
		return( 0 );
	}

};

void DexServices::Disconnect( void ) {
	closesocket( dexSocket );
	WSACleanup();
	printDateTime( log );
	fprintf( log, " Connection closed.\n" );
}

int DexServices::Send( const unsigned char *packet, int size ) {

	int retval = send( dexSocket, (const char *) packet, size, 0);
	if ( retval == SOCKET_ERROR )
	{
		fOutputDebugString( "Client: send() failed: error %d.\n", WSAGetLastError());
		fOutputDebugString( "XSACleanup() ... " );
		WSACleanup();
		fOutputDebugString( "OK\n" );
		return( -1 );
	}
	return retval;

}

int DexServices::SendTaskInfo( int user, int protocol, int task, int step ) {

	// A buffer to hold the string of bytes that form the packet.
	u8 packet[1024];
	// An object that serializes the data destined for DEX housekeeping telemetry packets.
	HK_packet hk;

	// Fill the packet with info.
	hk.current_protocol = protocol;
	hk.current_user = user;
	hk.current_task = task;
	hk.current_step = step;

	// These two items are currently unused.
	hk.motiontracker_status = 99;
	hk.scriptengine_status = 99;

	// Turn the data structure into a string of bytes with header etc.
	u32 size = hk.serialize( packet );

	// Send it to DEX.
	Send( packet, size );

	if ( log ) {
		printDateTime( log );
		fprintf( log, " Sent task info: %d %d %d %d   %d %d.\n", 
			user, protocol, task, step, hk.motiontracker_status, hk.scriptengine_status );
	}

	return 0 ;
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
		fOutputDebugString( "Warning: Truncating tag %s to %s\n", tag, cam.tag );
	}

	// Turn the data structure into a string of bytes with header etc.
	u32 size = cam.serialize( packet );

	// Send it to DEX.
	Send( packet, size );

	if ( log ) {
		printDateTime( log );
		fprintf( log, " Snapped picture.\n" );
	}
	return 0 ;
}
