// GraspVR.h

#pragma once
#include "GraspGLObjects.h"

namespace Grasp {

	class GraspVR : public GraspGLObjects
	{

	private:

		PsyPhy::OculusViewpoint *viewpoint;

		

	public:

		int Projectile_Timer;
		Vector3 Init_Projectile_Position;

		GraspVR( void ) {}
		~GraspVR( void ) {}

		void Initialize( HINSTANCE hinst );
		

		// Draw the objects that are used during VR.
		void Draw( void );

		// A rendering loop that allows one to toggle on and off the various VR objects.
		void DebugLoop( void );
		void Release( void );

	};
}
