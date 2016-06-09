#pragma once 

#include "../OculusInterface/OculusInterface.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"

// Allows one to use the PsyPhy OpenGL object libraries with the Oculus.
// Using this viewpoint, instead of the standard OpenGLObjects viewpoint sets up the GL transformations
//  to be compatible with the OculusMapper windows and buffers.

namespace PsyPhy {

class OculusViewpoint : public Viewpoint {

private:

	OculusMapper *oculusMapper;
	double upTan;
	double downTan;
	double leftTan;
	double rightTan;

public:

	// Sets up the viewpoint parameters compatible with the Oculus viewing frustrum.
	// Note that the constant values here are based on empirical observation with a DK2.
	// They should preferably be taken from the Oculus configuration manager via the OVR system.
	// But this is good enough for now.
	OculusViewpoint( OculusMapper *oculusMapper = nullptr, double ipd = 0.06, double nearest = 0.01, double farthest = 1000.0 );
	void Apply( int eye );
	void OculusViewpoint::GetEyeProjections ( int eye, OVR::Matrix4f *view, OVR::Matrix4f *projection );

};

};