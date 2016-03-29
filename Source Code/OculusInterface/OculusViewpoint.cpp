#include "OculusInterface.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "OculusViewpoint.h"

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
	int eye_buffer_width = 1162;
	int eye_buffer_height = 1464;
	double aspect = (double) eye_buffer_width / (double) eye_buffer_height;
	
	switch( eye ) {

	case LEFT_EYE:
		glViewport( 0, 0, eye_buffer_width, eye_buffer_height );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( fov, aspect, nearest, farthest );
		glTranslated( +ipd / 2, 0.0, 0.0 );

		//// Get ready to draw into one of the eyes.
 		oculusMapper->SelectEye( eye );

		break;

	case  RIGHT_EYE:
		glViewport( 0, 0, eye_buffer_width, eye_buffer_height );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( fov, aspect, nearest, farthest );
		glTranslated( -ipd / 2, 0.0, 0.0 );

		//// Get ready to draw into one of the eyes.
 		oculusMapper->SelectEye( eye );

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
