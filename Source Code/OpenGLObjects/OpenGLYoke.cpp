
#include "OpenGLYoke.h"

using namespace PsyPhy;

void Yoke::SetPosition( double x, double y, double z ) {
	OpenGLObject::SetPosition( x, y, z );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetPosition( x, y, z );
}
void Yoke::SetPosition( const Vector3 p ) {
	OpenGLObject::SetPosition( p );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetPosition( p );
}

void Yoke::SetOrientation( const Quaternion q ) {
	OpenGLObject::SetOrientation( q );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetOrientation( q );
}
void Yoke::SetOrientation( const Matrix3x3 m ) {
	OpenGLObject::SetOrientation( m );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetOrientation( m );
}
void Yoke::SetOrientation( double angle, const Vector3 axis ) {
	OpenGLObject::SetOrientation( angle, axis );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetOrientation( angle, axis );
}
void Yoke::SetOrientation( double roll, double pitch, double yaw ) {
	OpenGLObject::SetOrientation( roll, pitch, yaw );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetOrientation( roll, pitch, yaw );
}
void Yoke::SetPose(Pose &pose ) {
	OpenGLObject::SetPose( pose );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetPose( pose );
}


void Yoke::SetColor( float r, float g, float b, float a ) {
	OpenGLObject::SetColor( r, g, b, a );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetColor( r, g, b, a );
}
void Yoke::SetColor( int index ) {
	OpenGLObject::SetColor( index );
	for ( int obj = 0; obj < components; obj++ ) component[obj]->SetColor( index );
}	

// Drawing involves calling Draw() on each object.
// You can disable the entire set of yoked objects from being drawn all together with Yoke::Draw.
// But it will not affect the enabled state of the individual components.

void Yoke::PrepDraw( void ) {
	if ( ! enabled ) return;
	for ( int obj = 0; obj < components; obj++ ) component[obj]->PrepDraw();
}
void Yoke::Draw( void ) {
	if ( ! enabled ) return;
	for ( int obj = 0; obj < components; obj++ ) component[obj]->Draw();
}
void Yoke::FinishDraw( void ) {
	if ( ! enabled ) return;
	for ( int obj = 0; obj < components; obj++ ) component[obj]->FinishDraw();
}