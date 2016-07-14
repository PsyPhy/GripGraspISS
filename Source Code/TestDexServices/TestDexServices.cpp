// TestDexServices.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <conio.h>

#include "../DexServices/DexServices.h"

using namespace Grasp;

int main(int argc, char *argv[])
{

	DexServices *dex;

	fprintf( stderr, "DexServices: Instantiating ... " );
	dex = new DexServices;
	dex->Connect();
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "DexServices: Initializing ... " );
	dex->Initialize();
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "DexServices: Send dummy HK packet ... " );
	dex->SendTaskInfo( 110, 220, 33, 44 );
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "DexServices: Snap a picture ... " );
	dex->SnapPicture( "JOE" );
	fprintf( stderr, "OK.\n" );

	Sleep( 600 );

	fprintf( stderr, "DexServices: Snap a picture ... " );
	dex->SnapPicture( "JOSEPHMCINTYRE" );
	fprintf( stderr, "OK.\n" );


	fprintf( stderr, "DexServices: Disconnecting ... " );
	dex->Disconnect();
	fprintf( stderr, "OK.\n" );

	fprintf( stderr, "\nPress <Return> to exit ... " );
	_getch();

	return 0;
}

