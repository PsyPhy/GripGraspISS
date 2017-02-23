///
/// Module:	GripGraspVersionControl (GripMMI, Grasp, GraspMMI)
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		18 February 2017
/// Modification History:	see https://github.com/PsyPhy/GripMMI
///
/// Copyright (c) 2014, 2015, 2016, 2017 PsyPhy Consulting
///

/// This module creates a library at the bottom of the source code hierarchy.
/// When compiled, it defines version and build info for the different project.
/// Other modules link to it to get the shared version and build info.

#include "../ForceRebuild/CommonTimestamp.h"

#ifdef _DEBUG 
char *GripGraspBuildInfo = __COMMONTIMESTAMP__ " debug";
#else
char *GripGraspBuildInfo = __COMMONTIMESTAMP__;
#endif
