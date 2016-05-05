// This is the main DLL file.

#include "stdafx.h"
#include "../Useful/fMessageBox.h"
#include "../OculusInterface/OculusInterface.h"
#include "GraspVR.h"

using namespace OVR;
using namespace Grasp;


void GraspVR::Initialize( HINSTANCE hinst ) {

	// Interface to OpenGL windows and HMD.
//	static OculusDisplayOGL oculusDisplay;

	// Mapping and rendering in Oculus.
//	static OculusMapper oculusMapper;

	ovrResult result;
#if 0
   // Initializes LibOVR, and the Rift
    OVR::System::Init();
    result = ovr_Initialize( nullptr );
    fAbortMessageOnCondition( OVR_FAILURE( result ), "GraspVR", "Failed to initialize libOVR." );

	// Initialize the Oculus-enabled Windows platform.
    fAbortMessageOnCondition( !oculusDisplay.InitWindow( hinst, L"GraspVR"), "GraspVR", "Failed to open window." );

	// Initialize the interface to the Oculus HMD.
	result = oculusMapper.Initialize( &oculusDisplay );
	fAbortMessageOnCondition( OVR_FAILURE( result ), "GraspVR", "Failed to initialize libOVR." );
#endif
}