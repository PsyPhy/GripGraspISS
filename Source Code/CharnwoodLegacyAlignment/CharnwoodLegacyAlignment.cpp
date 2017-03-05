// CharnwoodLegacyAlignment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Include/codasys.h"
#include "../CodaLegacySDK/CodaUtilities.h"

int main(int argc, char *argv[])
{

	// Initialize the CODA hardware.
	CodaConnectStartAndPrepare( NULL );		// Will get the host address from an Environment variable.
	fprintf( stderr, "Started ... " );
	getchar();
	CodaQuit();
	fprintf( stderr, "Finished\n" );
	getchar();

	return 0;
}

