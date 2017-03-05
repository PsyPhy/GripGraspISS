#include "OculusInterface.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "OculusViewpoint.h"
#include "../Useful/fMessageBox.h"

// Allows one to use the PsyPhy OpenGL object libraries with the Oculus.
// Using this viewpoint, instead of the standard OpenGLObjects viewpoint sets up the GL transformations
//  to be compatible with the OculusMapper windows and buffers.

using namespace PsyPhy;

OculusViewpoint::OculusViewpoint( OculusMapper *oculusMapper, double ipd, double nearest, double farthest  ) {
	this->oculusMapper = oculusMapper;
	this->ipd = ipd;
	// An OculusViewpoint is going to define the viewing frustum based on the characteristics of the 
	// device. So we set the more standard fov to zero to see what happens.
	this->fov = 0.0;
	this->nearest = nearest;
	this->farthest = farthest;
	Viewpoint( ipd, fov, nearest, farthest );
}

void OculusViewpoint::Apply( int eye ) {

	// Set the size of the viewport into GL. The OVR system creates a bitmap of this size for each eye.
	int eye_buffer_width = oculusMapper->idealTextureSize.w;
	int eye_buffer_height = oculusMapper->idealTextureSize.h;
	double aspect = (double) eye_buffer_width / (double) eye_buffer_height;
	// Use crop to speed up rendering. Not a nice solution.
	int crop = 0;	

	double up_tan = oculusMapper->hmdDesc.DefaultEyeFov[eye].UpTan;
	double down_tan = oculusMapper->hmdDesc.DefaultEyeFov[eye].DownTan;
	double left_tan = oculusMapper->hmdDesc.DefaultEyeFov[eye].LeftTan;
	double right_tan = oculusMapper->hmdDesc.DefaultEyeFov[eye].RightTan;

	glViewport( crop, crop, eye_buffer_width - crop * 2, eye_buffer_height - crop * 2 );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum( - nearest * left_tan, nearest * right_tan, - nearest * down_tan, nearest * up_tan, nearest, farthest );

	switch( eye ) {

	case LEFT_EYE:
		glTranslated( + ipd / 2, 0.0, 0.0 );
		break;

	case  RIGHT_EYE:
		glTranslated(  - ipd / 2, 0.0, 0.0 );
		break;

	default:
		break;

	}

	// This additional translation and rotation allows us to change the 
	// control point of the viewpoint, i.e. where it is looking
	// when it's postion is zero.
	glMultMatrixd( gl_attitude );
	glTranslatedv( gl_offset );

	// Set up the local reference frame according to
	// the viewpoint's position and orientation.
	glMultMatrixd( gl_orientation );
	glTranslatedv( gl_position );

};

// Get the transforms that will map from world space to the space of each eye.
// Outputs are the view and projection matrices for the specified eye.
void OculusViewpoint::GetEyeProjections ( int eye, OVR::Matrix4f *view, OVR::Matrix4f *projection ) {

	fAbortMessageOnCondition( true, "OculusViewpoint::GetEyeProjections", "This does not yet work!" );
	/*

	The code here is taken from OculusRoomTiny. What I want to do is to implement the same calculation
	based on the position, offset, orientation and attitude of the viewpoint. But I need to think more.

	OVR::Vector3f shiftedEyePosition = position + orientation.Transform( EyeRenderPose[eye].Position );
	OVR::Matrix4f eyeOrientation = orientation * OVR::Matrix4f( EyeRenderPose[eye].Orientation );
    OVR::Vector3f up = eyeOrientation.Transform( OVR::Vector3f( 0, 1, 0 ) );
    OVR::Vector3f forward = eyeOrientation.Transform( OVR::Vector3f( 0, 0, -1 ) );

    *view = OVR::Matrix4f::LookAtRH( shiftedEyePosition, shiftedEyePosition + forward, up );
    *projection = ovrMatrix4f_Projection( hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_RightHanded );
	*/

}
