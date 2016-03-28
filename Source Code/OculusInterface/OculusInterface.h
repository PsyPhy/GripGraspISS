// OculusInterface.h

#pragma once

// The following includes comes from the Oculus OVR source files.
// The path is set via the user macro $(OVRSDKROOT) and via the property pages
//  in the VS2010 project files. I was able to modify $(OVRSDKROOT) by editing 
//  OVRRootPath.props. I could not figure out how to do it within VS2010.
#include "Kernel/OVR_System.h"
#include "OVR_CAPI_GL.h"

#include "OculusWIN32.h"
#include "OculusMapper.h"