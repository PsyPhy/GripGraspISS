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

#include "stdafx.h"
#include "../GripGraspVersionControl/GripGraspVersionControl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf( "Source release: %s Build Info: %s", GripGraspSourceRelease, GripGraspBuildInfo );
	return 0;
}

