/*!
*******************************************************************************
* \file TMclient.cpp
* \brief Dummy TM client application for GRASP test purposes.
*
* <CENTER><B>
* QinetiQ Space N.V.</B><BR>
* Hogenakkerhoekstraat 9<BR>
* B-9150 Kruibeke<BR>
* Belgium* +32 3 250 14 14<BR>
* </CENTER>
*
* \par Copyright(c), QinetiQ Space N.V. 2016 :
* <BR>
* The software contained within this file is proprietary and may
* not be copied, modified, compiled or used for whatever purpose.
* People getting hold of this file, must immediately delete it
* from any data carrier or physically destroy all tangible copies
* in their possession.
*
* Note: File under source code control; refer to project under
*  http://svn.orion.local:18080/viewvc/dex/ (web browser) or
*  http://svn.orion.local:18080/svn/dex (SVN client)
* for change history
******************************************************************************/
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
extern "C" {
	int gettimeofday(struct timeval * tp, struct timezone * tzp);
}
#include <windows.h>
#include <mmsystem.h>

#include "TMdata.h"


#define DEFAULT_PORT 2128
#define DEFAULT_SERVER "127.0.0.1"

const char *version = "0.0.1";

void Usage(char *progname)
{
	fprintf(stderr,"Usage: %s -n [server name/IP] -p [port_num]\n", progname);
	fprintf(stderr,"\t- server is the IP address of server\n");
	fprintf(stderr,"\t- port_num is the port to listen on\n");
	fprintf(stderr,"\t- The defaults are %s and %u\n",DEFAULT_SERVER, DEFAULT_PORT);
	WSACleanup();
	exit(1);
}


bool auto_hk=true;
bool auto_rt=true;
bool auto_cam=false;
bool manual_mode = false;
bool exit_req = false;
u32 verbosity=1;
double hk_period = 1.0;
double rt_period = 0.5;
double cam_period = 0.5;
u32 hk_count=0;
u32 rt_count=0;
u32 cam_count=0;

const double min_interval=0.001;
const double max_interval=10000;

LARGE_INTEGER pfreq, ptime;
LONGLONG hk_expiry, rt_expiry, cam_expiry;

void printTstp()
{
	struct timeval tv;
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64];

	gettimeofday(&tv, NULL);
	nowtime = tv.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	printf("%s.%03ld  ", tmbuf, tv.tv_usec/1000);
}

void sendHK(SOCKET  &conn_socket)
{
	HK_packet hkPkt;
	u8 buf[1024];

	hkPkt.FillPattern(); // HK fields sent at 1 Hz
	u32 size = hkPkt.serialize(buf);
	int retval = send(conn_socket, (const char *)buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		fprintf(stderr,"Client: send() failed: error %d.\n", WSAGetLastError());
		WSACleanup();
		exit(-1);
	}
	else
	{
		hk_count++;
		if(verbosity>2) {
			fprintf( stderr, "HK: %d %d %d %d\n", hkPkt.current_user, hkPkt.current_protocol, hkPkt.current_task, hkPkt.current_step );
		}
		if(verbosity>1)
		{
			printTstp();
		}
		if(verbosity>0)
		{
			printf("Client: HK TX OK.\n");
			fflush(stdout);
		}
	}
}

void sendCam(SOCKET  &conn_socket)
{
	CameraTrigger_packet camPkt;
	u8 buf[1024];

	camPkt.FillPattern();
	u32 size = camPkt.serialize(buf);
	int retval = send(conn_socket, (const char *)buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		fprintf(stderr,"Client: send() failed: error %d.\n", WSAGetLastError());
		WSACleanup();
		exit(-1);
	}
	else
	{
		cam_count++;
		if(verbosity>1)
		{
			printTstp();
		}
		if(verbosity>0)
		{
			printf("Client: CAM TX OK.\n");
			fflush(stdout);
		}
	}

}

void sendRT(SOCKET  &conn_socket)
{
	RT_packet rtPkt;
	u8 buf[1024];

	rtPkt.FillPattern(); // RT science data packet sent at 2 Hz
	u32 size = rtPkt.serialize(buf);
	int retval = send(conn_socket, (const char *)buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		fprintf(stderr,"Client: send() failed: error %d.\n", WSAGetLastError());
		WSACleanup();
		exit(-1);
	}
	else
	{
		rt_count++;
		if(verbosity>1)
		{
			printTstp();
		}
		if(verbosity>0)
		{
			printf("Client: RT TX OK.\n");
			fflush(stdout);
		}
	}
}


void printHelp()
{
	printf("?\tShow this help\n");
	printf("H\tIn auto mode, toggle housekeeping transmission.\n\tIn manual mode, send housekeeping packet.\n");
	printf("R\tIn auto mode, toggle real time TM transmission.\n\tIn manual mode, send real time TM packet.\n");
	printf("C\tIn auto mode, toggle camera trigger transmission.\n\tIn manual mode, send camera trigger packet.\n");
	printf("S\tDisplay status\n");
	printf("M\tToggle between manual and auto mode.\n");
	printf("I\tChange transmission interval\n");
	printf("V\tChange verbosity level\n");
	printf("Q\tQuit application\n");
	fflush(stdout);
}

void printStatus()
{
	printf("Mode                    : %s\n",manual_mode ? "MANUAL" : "AUTO");
	printf("Housekeeping TM         : %s  Period: %6.3lf s     Count = %u\n",auto_hk ? "ON " : "OFF", hk_period,hk_count);
	printf("Real time TM            : %s  Period: %6.3lf s     Count = %u\n",auto_rt ? "ON " : "OFF", rt_period,rt_count);
	printf("CAM trigger transmission: %s  Period: %6.3lf s     Count = %u\n",auto_cam ? "ON " : "OFF", cam_period,cam_count);
	fflush(stdout);
}


void getValue(const char *s, double *val)
{
	printf("%s",s);
	fflush(stdout);
	double input=*val;
	scanf("%lf",&input);
	if((input>=min_interval)&&(input<=max_interval))
	{
		*val=input;
	}
	else
	{
		printf("Invalid value\n");
		fflush(stdout);

	}
}

void changeInterval()
{
	printf("Change interval of (H)ousekeeping, (R)eal-Time Packet, (C)amera trigger ?\n");
	fflush(stdout);
	int c = _getch();
	switch(c)
	{
	case 'h':
	case 'H':
		getValue("Enter new HK TM interval: ",&hk_period);
		break;
	case 'r':
	case 'R':
		getValue("Enter new RT TM interval: ",&rt_period);
		break;
	case 'c':
	case 'C':
		getValue("Enter new camera trigger interval: ",&cam_period);
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	printf("Dummy TM client for GRASP, version %s\n",version);
	printf("Press '?' for runtime help\n");
	fflush(stdout);

	// default to localhost
	const char *server_name= DEFAULT_SERVER;
	u16 port = DEFAULT_PORT;
	int retval;
	u32 addr;
	int socket_type = SOCK_STREAM;
	struct sockaddr_in server;
	WSADATA wsaData;
	SOCKET  conn_socket;

	if (argc >1)
	{
		for(int i=1; i<argc; i++)
		{
			if ((argv[i][0] == '-') || (argv[i][0] == '/'))
			{
				switch(tolower(argv[i][1]))
				{
				case 'n':
					server_name = argv[++i];
					break;
				case 'p':
					port = atoi(argv[++i]);
					break;
				default:
					Usage(argv[0]);
					break;
				}
			}
			else
				Usage(argv[0]);
		}
	}


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
		printf("Client: WSAStartup() is OK.\n");
		fflush(stdout);
	}

	if (port == 0)
	{
		Usage(argv[0]);
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
		printf("Client: socket() is OK.\n");
		fflush(stdout);
	}
	// Set TCP_NODELAY to reduce jitter
	s32 flag = 1;
	setsockopt(conn_socket,            /* socket affected */
		IPPROTO_TCP,     /* set option at TCP level */
		TCP_NODELAY,     /* name of option */
		(sz *) &flag,  /* the cast is historical
					   cruft */
					   sizeof(s32));    /* length of option value */

	printf("Client: Client connecting to: %s.\n", server_name);
	fflush(stdout);

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
		printf("Client: connect() is OK.\n");
		fflush(stdout);
	}

	// Initialize timer stuff
	QueryPerformanceFrequency(&pfreq);
	QueryPerformanceCounter(&ptime);
	hk_expiry = ptime.QuadPart;
	rt_expiry = ptime.QuadPart;
	cam_expiry = ptime.QuadPart;
	timeBeginPeriod(1); // Increase windows timer resolution

	while(!exit_req)
	{
		if( _kbhit() )
		{
			int c = _getch();
			switch(c)
			{
			case 'h':
			case 'H':
				if(manual_mode)
				{
					sendHK(conn_socket);
				}
				else
				{
					auto_hk=!auto_hk;
					printf("Housekeeping TM is %s\n",auto_hk ? "ON" : "OFF");
					fflush(stdout);
					if(auto_hk)
					{
						hk_expiry = ptime.QuadPart;
					}
				}
				break;
			case 'r':
			case 'R':
				if(manual_mode)
				{
					sendRT(conn_socket);
				}
				else
				{
					auto_rt=!auto_rt;
					printf("Real time TM is %s\n",auto_rt ? "ON" : "OFF");
					fflush(stdout);
					if(auto_rt)
					{
						rt_expiry = ptime.QuadPart;
					}
				}
				break;
			case 'c':
			case 'C':
				if(manual_mode)
				{
					sendCam(conn_socket);
				}
				else
				{
					auto_cam=!auto_cam;
					printf("CAM trigger transmission is %s\n",auto_cam ? "ON" : "OFF");
					fflush(stdout);
					if(auto_cam)
					{
						cam_expiry = ptime.QuadPart;
					}
				}
				break;
			case 's':
			case 'S':
				printStatus();
				break;
			case 'm':
			case 'M':
				manual_mode=!manual_mode;
				printf("Mode is %s\n",manual_mode ? "MANUAL" : "AUTO");
				fflush(stdout);
				hk_expiry= ptime.QuadPart;
				rt_expiry= ptime.QuadPart;
				cam_expiry= ptime.QuadPart;
				break;
			case 'i':
			case 'I':
				changeInterval();
				QueryPerformanceCounter(&ptime);
				hk_expiry= ptime.QuadPart;
				rt_expiry= ptime.QuadPart;
				cam_expiry= ptime.QuadPart;
				break;
			case 'v':
			case 'V':
				verbosity = (verbosity+1)%4;
				printf("Verbosity level is %u\n",verbosity);
				fflush(stdout);
				break;
			case 'q':
			case 'Q':
				exit_req=true;
				break;
			default:
				printHelp();
				break;
			}
		}

		// Update current time
		QueryPerformanceCounter(&ptime);

		if(!manual_mode)
		{
			// Transmit auto generated packets if their time is due

			if(auto_hk && (hk_expiry<=ptime.QuadPart))
			{
				sendHK(conn_socket);
				hk_expiry += (LONGLONG)(hk_period * pfreq.QuadPart);
			}
			if(auto_rt && (rt_expiry<=ptime.QuadPart))
			{
				sendRT(conn_socket);
				rt_expiry += (LONGLONG)(rt_period * pfreq.QuadPart);
			}
			if(auto_cam && (cam_expiry<=ptime.QuadPart))
			{
				sendCam(conn_socket);
				cam_expiry += (LONGLONG)(cam_period * pfreq.QuadPart);
			}
		}

		Sleep(1);

	}

	// Clean up
	timeEndPeriod(1U);
	closesocket(conn_socket);
	WSACleanup();



	return 0;

}
