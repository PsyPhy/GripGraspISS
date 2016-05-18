
#include "../Useful/fMessageBox.h"
#include "OpenGLObjects.h"


namespace PsyPhy {

/*
 * A collection of OpenGL graphics objects that move together and can
 *  be drawn at the same time. 
 */

class Yoke : public Assembly {

private:

	void Unsupported( char *message ) { fAbortMessage( "Yoke" "Yoke class: %s action unsupported.", message ); }

protected:

public:

	void  PrepDraw();
	void  Draw();
	void  FinishDraw();

	void  SetPosition( const Vector3 p );
	void  SetPosition( double x, double y, double z );

	void  SetOrientation( const Quaternion q );
	void  SetOrientation( const Matrix3x3 m );
	void  SetOrientation( double angle, const Vector3 axis );
	void  SetOrientation( double roll, double pitch, double yaw );
	void  GetOrientation( Matrix3x3 m );

	void SetPose( Pose &pose );

	void  SetAttitude( Matrix3x3 m ){ Unsupported( "SetAttitude" ); }
	void  SetAttitude( double angle, const Vector3 axis ){ Unsupported( "SetAttitude" ); }
	void  SetAttitude( double roll, double pitch, double yaw ){ Unsupported( "SetAttitude" ); }
	void  GetAttitude( Matrix3x3 m ){ Unsupported( "GetAttitude" ); }

	void  SetOffset( const Vector3 p ) { Unsupported( "SetOffset" ); }
	void  SetOffset( double x, double y, double z ) { Unsupported( "SetOffset" ); }
	void  GetOffset( Vector3 p ) { Unsupported( "GetOffset" ); }

	void SetColor( float r, float g, float b, float a = 1.0 );
	void SetColor( int index );

	Yoke( void ) { components = 0; enabled = true; };
	~Yoke( void ) {};


};

};


