///
/// Module:	GripsGraspShowVersionInfo
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		18 December 2014
/// Modification History:	see https://github.com/PsyPhy/GripMMI
///
/// Copyright (c) 2014, 2015, 2017 PsyPhy Consulting
///

/// This module creates a console application that simply outputs the current
///  release and build info to stdout. It is used by other  
///  makefile projects to write release information into a human readable text 
///  file to be included with the executables and other files when GripMMI is released.
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "../GripGraspVersionControl/GripGraspVersionControl.h"

int main(int argc, char *argv[])
{
	if ( argc < 1 ) printf( "Source release: %s Build Info: %s", GripGraspSourceRelease, GripGraspBuildInfo );
	else {
		int arg;
		unsigned int i;
		char tag[256];
		for ( arg = 1; arg < argc; arg++ ) {
			if ( !strcmp( "-b", argv[arg] ) ) strcpy( tag, GripGraspBuildInfo );
			if ( !strcmp( "-r", argv[arg] ) ) strcpy( tag, GripGraspSourceRelease );
			for ( i = 0; i < strlen( tag ); i++ ) if ( tag[i] == ':' ) tag[i] = '-';
			printf( "%s", tag );
		}
	}
	return 0;
}

