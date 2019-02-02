#pragma once

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"

#include "GraspData.h"


namespace PsyPhy {

	using namespace PsyPhy;
	using namespace Grasp;

	double tablet_width = 500.0;
	double tablet_length = 500.0;
	double tablet_thickness = 100.0;

	GLfloat tablet_color[4] = { .5f, .35f, .25f, 1.0f };
	GLfloat phantom_tablet_color[4] = { .25f, .85f, .25f, 0.25f };
	GLfloat manipulandum_color[4] = { .5f, .5f, .5f, 1.0f };
	GLfloat wrist_color[4] = { .35f, .35f, .35f, 1.0f };

	public class GripTablet : public Assembly {

	public:

		GripTablet( void ) {

			// Create objects to represent the GRIP hardware.
			Slab *slab;
			slab = new Slab( tablet_width / 2.0, tablet_length, 0.75 * tablet_thickness );
			slab->SetPosition( 0.265 * tablet_width, 0.0,  tablet_thickness / 2.0 );
			AddComponent( slab );
			slab = new Slab( tablet_width / 2.0, tablet_length, tablet_thickness );
			slab->SetPosition( - 0.265 * tablet_width, 0.0, 0.0 );
			AddComponent( slab );

			Slab *screen = new Slab( 0.35 * tablet_width, 0.55 * tablet_length, 20.0 );
			screen->SetColor( CYAN );
			screen->SetPosition( 0.265 * tablet_width, 0.175 * tablet_length, tablet_thickness );
			AddComponent( screen );

			Assembly *mast = new Assembly();
			Sphere *target;
			double mast_width = 25.0;
			double mast_height = 600.0;
			slab = new Slab( mast_width, mast_width, mast_height );
			slab->SetPosition( 0.0, 0.0, mast_height / 2.0 + tablet_thickness / 2.0 );
			mast->AddComponent( slab );
			for ( int i = 0; i < 11; i++ ) {
				target = new Sphere( 10.0 );
				target->SetColor( Translucid( RED ) );
				target->SetPosition( 0.0, mast_width, i * 50.0 + 100.0 );
				mast->AddComponent( target );
			}
			mast->SetPosition( - 0.45 * tablet_width, - 0.20 * tablet_length, 0.0 );
			AddComponent( mast );
			SetAttitude( 0.0, 90.0, 0.0 );
			SetOffset( tablet_width / 2.0, - tablet_thickness / 2.0, - tablet_length / 2.0 );
			SetColor( tablet_color );

		}

	};

	public class GripManipulandum : public Assembly {

	public:

		GripManipulandum( void ) {

			double profile[][2] = { { 13.0, -25.0 }, { -13.0, -25.0 }, { -13.0, 25.0 }, { 5.0, 25.0 }, { 13.0, 18.0 } }; 

			// Create objects to represent the GRIP manipulandum.
			Extrusion *extrusion = new Extrusion( 25.0, profile, 5 );
			extrusion->SetColor( manipulandum_color );
			AddComponent( extrusion );

		}

	};

	public class GripWrist : public Assembly {

	public:

		GripWrist( void ) {

			Cylinder *cone = new Cylinder( 30.0, 24.0, 50.0 );
			cone->SetOrientation( 0.0, 90.0, 0.0 );
			cone->SetColor( wrist_color );
			AddComponent( cone );

		}

	};

}

