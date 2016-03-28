
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"


// Allows one to use the PsyPhy OpenGL object libraries with the Oculus.
// Using this viewpoint, instead of the standard OpenGLObjects viewpoint sets up the GL transformations
//  to be compatible with the OculusMapper windows and buffers.

namespace PsyPhy {

class OculusViewpoint : public Viewpoint {

public:

	// Sets up the viewpoint parameters compatible with the Oculus viewing frustrum.
	// Note that the constant values here are based on empirical observation with a DK2.
	// They should preferably be taken from the Oculus configuration manager via the OVR system.
	// But this is good enough for now.
	OculusViewpoint( double ipd = 0.06, double fov = 106.0, double nearest = 0.01, double farthest = 1000.0 ) {
		this->ipd = ipd;
		this->fov = fov;
		this->nearest = nearest;
		this->farthest = farthest;
		Viewpoint( ipd, fov, nearest, farthest );
	}

	void Apply( int eye ) {

		// Set the size of the viewport into GL. The OVR system creates a bitmap of this size for each eye.
		// Again, these constants are empirically drawn from a DK2 sample program.
		// They could probably be retrieved via the OVR system.
		int eye_buffer_width = 1162;
		int eye_buffer_height = 1464;
		double aspect = (double) eye_buffer_width / (double) eye_buffer_height;

		switch( eye ) {

		case PsyPhy::LEFT_EYE:
			glViewport( 0, 0, eye_buffer_width, eye_buffer_height );
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective( fov, aspect, nearest, farthest );
			glTranslated( - ipd / 2, 0.0, 0.0 );
			break;

		case  PsyPhy::RIGHT_EYE:
			glViewport( 0, 0, eye_buffer_width, eye_buffer_height );
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective( fov, aspect, nearest, farthest );
			glTranslated( ipd / 2, 0.0, 0.0 );
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

};

};