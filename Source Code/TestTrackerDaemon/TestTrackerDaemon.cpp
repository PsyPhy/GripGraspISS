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

#define BROADCAST_PORT		8888
#define BROADCAST_ADDRESS	"localhost"
#define BUFLEN 1024

int main( int argc, char *argv[] )
{

	char buf[BUFLEN];

	Sleep( 5000 );
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) fAbortMessage( "GraspTrackerDaemon", "Error creating socket." );

	struct sockaddr_in Recv_addr, si_other;
	Recv_addr.sin_family = AF_INET;
	Recv_addr.sin_port = htons(BROADCAST_PORT);
	Recv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (sockaddr*)&Recv_addr, sizeof(Recv_addr)) < 0)
	{
		closesocket(sock);
		fAbortMessage( "GraspTrackerDaemon", "Error binding socket." );		
	}

	while(1)
	{
		int recv_len;
		int slen;

        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
         
        //try to receive some data, this is a blocking call
		slen = sizeof( si_other );
        if ((recv_len = recvfrom( sock, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
        if ((recv_len = recvfrom( sock, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen )) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
		//Sleep(3000);
		//if (sendto(sock, "Joe", strlen( "Joe" ) + 1, 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
		//{
		//	closesocket(sock);
		//	fAbortMessage( "GraspTrackerDaemon", "Error on sendto." );		
		//}

		//fprintf( stderr, "message sent successfully\n" );

		if ( _kbhit() ) break;

	}

	_getch();
	closesocket(sock);

	WSACleanup();
	return 0;
	return 0;
}
