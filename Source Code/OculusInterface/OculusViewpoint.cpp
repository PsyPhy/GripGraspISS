#include "OculusInterface.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "OculusViewpoint.h"
#include "../Useful/fMessageBox.h"

// Allows one to use the PsyPhy OpenGL object libraries with the Oculus.
// Using this viewpoint, instead of the standard OpenGLObjects viewpoint sets up the GL transformations
//  to be compatible with the OculusMapper windows and buffers.

using namespace PsyPhy;

OculusViewpoint::OculusViewpoint( OculusMapper *oculusMapper, double ipd, double fov, double nearest, double farthest  ) {
	this->oculusMapper = oculusMapper;
	this->ipd = ipd;
	this->fov = fov;
	this->nearest = nearest;
	this->farthest = farthest;
	Viewpoint( ipd, fov, nearest, farthest );
}

void OculusViewpoint::Apply( int eye ) {

	// Set the size of the viewport into GL. The OVR system creates a bitmap of this size for each eye.
	// Again, these constants are empirically drawn from a DK2 sample program.
	// They could probably be retrieved via the OVR system.
	int eye_buffer_width = oculusMapper->idealTextureSize.w;
	int eye_buffer_height = oculusMapper->idealTextureSize.h;
	double aspect = (double) eye_buffer_width / (double) eye_buffer_height;
	// Use crop to speed up rendering. Not a nice solution.
	int crop = 0;	
	switch( eye ) {

	case LEFT_EYE:
		glViewport( crop, crop, eye_buffer_width - crop * 2, eye_buffer_height - crop * 2 );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( fov, aspect, nearest, farthest );
		glTranslated( +ipd / 2, 0.0, 0.0 );
		break;

	case  RIGHT_EYE:
		glViewport( crop, crop, eye_buffer_width - crop * 2, eye_buffer_height - crop * 2 );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( fov, aspect, nearest, farthest );
		glTranslated( -ipd / 2, 0.0, 0.0 );
		break;

	default:
		glViewport( 0, 0, eye_buffer_width, eye_buffer_height );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( fov, aspect, nearest, farthest );
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
