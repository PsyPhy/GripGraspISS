// TestTrackerDaemon.cpp : main project file.

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include<winsock2.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

#include "../GraspTrackerDaemon/GraspTrackerDaemon.h"

GraspTrackerRecord record;

int main( int argc, char *argv[] )
{

	Sleep( 500 );
	WSADATA wsaData;
	WSAStartup( MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) fAbortMessage( "TestTrackerDaemon", "Error creating socket." );
		
	struct sockaddr_in Recv_addr, si_other;
	Recv_addr.sin_family = AF_INET;
	Recv_addr.sin_port = htons( TRACKER_DAEMON_PORT );
	Recv_addr.sin_addr.s_addr = INADDR_ANY;
	int slen = sizeof( si_other );
	BOOL enabled = TRUE;
	if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&enabled, sizeof(BOOL)) < 0 ) 
	{
		closesocket(sock);
		fAbortMessage( "TestTrackerDaemon", "Error setting socket options." );		
	}

	if ( bind(sock, (sockaddr*)&Recv_addr, sizeof(Recv_addr)) < 0)
	{
		int error_value = WSAGetLastError();
		closesocket(sock);
		fAbortMessage( "TestTrackerDaemon", "Error binding socket (%d).", error_value );		
	}
	unsigned long noBlock = 1;
	ioctlsocket( sock, FIONBIO, &noBlock );

	while(1)
	{
		int recv_len;
		static int count = 0;

        //clear the buffer by filling null, it might have previously received data
        memset( &record, 0, sizeof( record ));
  
#if 0
        //try to receive some data, this is a blocking call
		slen = sizeof( si_other );
        if ((recv_len = recvfrom( sock, (char *) &record, sizeof( record ), 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            fAbortMessage( "TestTrackerDaemon", "recvfrom() failed with error code : %d" , WSAGetLastError() );
        }
#else
        // Clear any pending inputs and only take the last one.
		int packet_count;
        for ( packet_count = 0; true; packet_count++ ) {
			recv_len = recvfrom( sock, (char *) &record, sizeof( record ), 0, (struct sockaddr *) &si_other, &slen);
			int nError = WSAGetLastError();
			if ( nError == WSAEWOULDBLOCK ) break;
			if ( nError != 0 ) fAbortMessage( "TestTrackerDaemon", "recvfrom() failed with error code : %d" , nError );
			if ( recv_len != sizeof( record ) ) fAbortMessage( "TestTrackerDaemon", "recvfrom() returned unexpected byte count: %d vs. %d" , recv_len, sizeof( record ) );
        }
#endif
		if ( packet_count > 0 ) {
			printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			printf("Data: %8d %8d\n" , count++, record.count );
		}
 		if ( _kbhit() ) {
			_getch();
			char *reset = "RESET";
		}

		Sleep( 1000 );

	}

	closesocket(sock);

	WSACleanup();
	return 0;
	return 0;
}
