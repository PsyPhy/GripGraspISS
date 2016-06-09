// Module: DexServices
// This is the main DLL file.

#include "stdafx.h"
#include <winsock2.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "../Useful/fOutputDebugString.h"

#include "GripPackets.h"
#include "DexServices.h"

using namespace Grasp;

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

	dex_socket = socket(AF_INET, socket_type, 0); /* Open a socket */

	if ( dex_socket < 0 )
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
	setsockopt( dex_socket,         /* socket affected */
		IPPROTO_TCP,				/* set option at TCP level */
		TCP_NODELAY,				/* name of option */
		(char *) &flag,					
		sizeof( flag ));			/* length of option value */

	fOutputDebugString( "Client: Client connecting to: %s.\n", server_name );

	if (connect( dex_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)

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
	closesocket( dex_socket );
	WSACleanup();
}

int DexServices::Send( const char *packet, int size ) {

	int retval = send( dex_socket, packet, size, 0);
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

int DexServices::SendTaskInfo( int subject, int protocol, int task, int step ) {

	GripHealthAndStatusInfo info;
	EPMTelemetryPacket		packet;


	InsertGripHealthAndStatusInfo( &packet, &info );
	Send( (char *) &packet, sizeof( packet ) );


	return 0 ;
}
