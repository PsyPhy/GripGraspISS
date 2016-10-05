// TestDexServices.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <conio.h>

#include "../Trackers/PoseTrackers.h"
#include "../DexServices/DexServices.h"

using namespace Grasp;

int main(int argc, char *argv[])
{

	DexServices *dex;

	fprintf( stderr, "DexServices: Instantiating ... " );
	dex = new DexServices;
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "DexServices: Initializing ... " );
	dex->Initialize( "TestDexServices.dxl" );
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "DexServices: Connecting ... " );
	dex->Connect();
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "DexServices: Send dummy HK packet ... " );
	for ( int i = 0; i < 100; i++ ) {
		if ( 0 < dex->SendTaskInfo( 110, 220, 33, 44 ) ) fprintf( stderr, "OK.\n" );
		else fprintf( stderr, "not sent.\n" );
		Sleep( 100 );
	}

	fprintf( stderr, "DexServices: Send dummy RT packets ... " );
	static MarkerFrame frames[2];
	for ( int i = 0; i < 1000; i++ ) {
		dex->AddDataSlice( 0x12345678, PsyPhy::NullTrackerPose, PsyPhy::NullTrackerPose, PsyPhy::NullTrackerPose, PsyPhy::NullTrackerPose, PsyPhy::NullTrackerPose, frames );
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

	fprintf( stderr, "\nPress <Return> to exit ... " );
	_getch();

	return 0;
}

