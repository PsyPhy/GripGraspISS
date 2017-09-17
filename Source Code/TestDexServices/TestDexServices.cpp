// TestDexServices.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "../Trackers/PoseTrackers.h"
#include "../DexServices/DexServices.h"

using namespace Grasp;

int main(int argc, char *argv[])
{

	DexServices *dex;

	fprintf( stderr, "DexServices: Instantiating ... " );
//	dex = new DexServicesByProxy( PROXY_DEX_SERVER, PROXY_DEX_PORT );
	dex = new DexServices();
	fprintf( stderr, "OK.\n" );

	int user = 10;
	int protocol = 100;
	int task = 101;
	int step = 50;
	int substep = STEP_READY_TO_EXECUTE;

	while ( true ) {

		fprintf( stderr, "DexServices: Waiting for keypress to start ... " );
		if ( 'x' == _getch() ) break;


		fprintf( stderr, "DexServices: Initializing ... " );
		dex->Initialize( "TestDexServices.dxl" );
		fprintf( stderr, "OK.\n" );

		fprintf( stderr, "DexServices: Connecting ... " );
		dex->Connect();
		fprintf( stderr, "OK.\n" );

		fprintf( stderr, "DexServices: Send dummy HK packet ... " );
		for ( int i = 0; i < 10; i++ ) {
			if ( 0 < dex->SendTaskInfo( user, protocol, task, step, substep ) ) fprintf( stderr, "OK.\n" );
			else fprintf( stderr, "not sent.\n" );
			Sleep( 100 );
		}

		fprintf( stderr, "DexServices: Send dummy RT packets ... " );
		static MarkerFrame frames[2];
		for ( int i = 0; i < 100; i++ ) {
			dex->AddTrackerSlice( PsyPhy::NullTrackerPose, PsyPhy::NullTrackerPose, PsyPhy::NullTrackerPose, frames );
			unsigned char buffer[100];
			dex->AddClientSlice( buffer, sizeof( buffer ) );
			Sleep( 1 );
		}
		fprintf( stderr, "OK.\n" );

		fprintf( stderr, "DexServices: Snap a picture ... " );
		if ( 0 < dex->SnapPicture( "JOE" ) ) fprintf( stderr, "OK.\n" );
		else fprintf( stderr, "not sent.\n" );

		Sleep( 600 );

		fprintf( stderr, "DexServices: Snap a picture ... " );
		if ( 0 < dex->SnapPicture( "JOSEPHMCINTYRE" ) ) fprintf( stderr, "OK.\n" );
		else fprintf( stderr, "not sent.\n" );

		fprintf( stderr, "DexServices: Disconnecting ... " );
		dex->Disconnect();
		fprintf( stderr, "OK.\n" );

		user++;
		protocol += 100;
		task = protocol + user;
		step++;
		substep = STEP_EXECUTING + user + protocol + task + step;

	}

	return 0;
}

