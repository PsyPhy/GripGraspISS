#include "stdafx.h"
#include <winsock2.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "../Useful/fOutputDebugString.h"

#define DEFAULT_PORT 2128
#define DEFAULT_SERVER "10.80.12.103"

int ConnectToDex ( void ) {

	// default to localhost
	const char *server_name= DEFAULT_SERVER;
	unsigned short port = DEFAULT_PORT;
	int retval;
	unsigned long addr;
	int socket_type = SOCK_STREAM;
	struct sockaddr_in server;
	WSADATA wsaData;
	SOCKET  conn_socket;

	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
		fprintf(stderr,"Client: WSAStartup() failed with error %d\n", retval);
		fprintf(stderr, "Press <Return> to continue. " );
		_getch();
		WSACleanup();
		return -1;
	}
	else
	{
		fprintf( stderr, "Client: WSAStartup() is OK.\n");
	}

	{ // Convert nnn.nnn address to a usable one
		addr = inet_addr(server_name);

		if(addr==INADDR_NONE)
		{
			fprintf(stderr,"Client: problem interpreting IP address.\n");
			fprintf(stderr, "Press <Return> to continue. " );
			_getch();
			WSACleanup();
			exit(1);
		}
	}

	// Copy the resolved information into the sockaddr_in structure

	memset(&server, 0, sizeof(server));
	memcpy(&(server.sin_addr), &addr, 4);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	conn_socket = socket(AF_INET, socket_type, 0); /* Open a socket */

	if (conn_socket <0 )
	{
		fprintf(stderr,"Client: Error Opening socket: Error %d\n", WSAGetLastError());
		fprintf(stderr, "Press <Return> to continue. " );
		_getch();
		WSACleanup();
		return -1;
	}
	else
	{
		fprintf( stderr, "Client: socket() is OK.\n" );
	}
	// Set TCP_NODELAY to reduce jitter
	unsigned int flag = 1;
	setsockopt(conn_socket,         /* socket affected */
		IPPROTO_TCP,				/* set option at TCP level */
		TCP_NODELAY,				/* name of option */
		(char *) &flag,					
		sizeof( flag ));			/* length of option value */

	fprintf( stderr, "Client: Client connecting to: %s.\n", server_name );

	if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)

	{
		fprintf(stderr,"Client: connect() failed: %d\n", WSAGetLastError());
		fprintf(stderr, "Press <Return> to continue. " );
		_getch();
		WSACleanup();
		return -1;
	}
	else
	{
		fOutputDebugString( "ConnectToDex() OK.\n" );
		fprintf( stderr, "Client: connect() is OK.\n" );
		return( 0 );
	}

}