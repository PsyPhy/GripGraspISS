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

	GLfloat right_tablet_color[4] = { 0.5f, .35f, .25f, 1.0f };
	GLfloat left_tablet_color[4] = { 0.5f, .35f, .25f, 1.0f };
	GLfloat mast_color[4] = { .3f, .22f, .22f, 1.0f };
	GLfloat phantom_tablet_color[4] = { .25f, .85f, .25f, 0.25f };
	GLfloat manipulandum_color[4] = { .5f, .5f, .5f, 1.0f };
	GLfloat wrist_color[4] = { .35f, .35f, .35f, 1.0f };

	public class GripTablet : public Assembly {

	public:

		GripTablet( void ) {

			// Create objects to represent the GRIP hardware.
			Assembly *left = new Assembly();
			Slab *slab;
			slab = new Slab( tablet_width / 2.0, tablet_length, 0.75 * tablet_thickness );
			slab->SetColor( left_tablet_color );
			left->AddComponent( slab );

			Slab *screen = new Slab( 0.35 * tablet_width, 0.40 * tablet_length, 20.0 );
			screen->SetColor( 0.1, 0.5, 0.5, 1.0 );
			screen->SetPosition( 0.0, 0.25 * tablet_length, tablet_thickness / 2.0 - 20.0 );
			left->AddComponent( screen );

			Assembly *brightness_knob = new Assembly();
			Cylinder *shaft = new Cylinder( 0.04 * tablet_width, 0.04 * tablet_width, 20.0 );
			shaft->SetColor( BLACK );
			brightness_knob->AddComponent( shaft );
			Circle *cap = new Circle( shaft->top_radius );
			cap->SetColor( BLACK );
			cap->SetPosition( 0.0, 0.0, shaft->height / 2.0 );
			brightness_knob->AddComponent( cap );
			brightness_knob->SetPosition( - 0.2 * tablet_width, - 0.05 * tablet_length, tablet_thickness / 2.0 );
			left->AddComponent( brightness_knob );

			Assembly *volume_knob = new Assembly();
			shaft = new Cylinder( 0.04 * tablet_width, 0.04 * tablet_width, 0.2 * tablet_thickness );
			shaft->SetColor( BLACK );
			volume_knob->AddComponent( shaft );
			cap = new Circle( shaft->top_radius );
			cap->SetColor( BLACK );
			cap->SetPosition( 0.0, 0.0, shaft->height / 2.0 );
			volume_knob->AddComponent( cap );
			volume_knob->SetPosition( - 0.1 * tablet_width, - 0.15 * tablet_length, tablet_thickness / 2.0 );
			left->AddComponent( volume_knob );
			AddComponent( left );
			left->SetPosition( 0.265 * tablet_width, 0.0,  tablet_thickness / 2.0 );

			Assembly *right = new Assembly();
			slab = new Slab( tablet_width / 2.0, tablet_length, tablet_thickness );
			slab->SetColor( right_tablet_color );
			right->AddComponent( slab );
			right->SetPosition( - 0.265 * tablet_width, 0.0, 0.0 );

			Assembly *mass;
			double mass_width = 35.0;
			for ( int i = -1; i < 2; i++ ) {
				mass = new Assembly();
				slab = new Slab( mass_width, mass_width, 1.5 * mass_width );
				slab->SetPosition( - mass_width, 0.0, 0.0 );
				mass->AddComponent( slab );
				slab = new Slab( mass_width, mass_width, 1.5 * mass_width );
				slab->SetPosition( mass_width, 0.0, 0.0 );
				mass->AddComponent( slab );
				slab = new Slab( mass_width, mass_width / 2.0, 1.5 * mass_width );
				slab->SetPosition( 0.0, 0.0, 0.0 );
				mass->AddComponent( slab );
				mass->SetPosition( 0.1 * tablet_width, tablet_length / 4.0 * i, tablet_thickness - mass_width );
				mass->SetColor( BLACK );
				right->AddComponent( mass );
			}
			AddComponent( right );


			Assembly *mast = new Assembly();
			Sphere *target;
			double mast_width = 25.0;
			double mast_height = 600.0;
			slab = new Slab( mast_width, mast_width, mast_height );
			slab->SetPosition( 0.0, 0.0, 0.0 );
			mast->AddComponent( slab );

			slab = new Slab( mast_width, 4.5 * mast_width, mast_width );
			slab->SetPosition( 0.0, - 2.0 * mast_width, mast_height / 2.0 - mast_width / 2.0);
			mast->AddComponent( slab );
			slab = new Slab( mast_width, 4.0 * mast_width, 6 * mast_width );
			slab->SetPosition( 0.0, - 2.5 * mast_width, mast_height / 2.0 - 4.0 * mast_width );
			slab->SetColor( BLUE );
			mast->AddComponent( slab );

			slab = new Slab( mast_width, 4.5 * mast_width, mast_width );
			slab->SetPosition( 0.0, - 2.0 * mast_width, - ( mast_height / 2.0 - mast_width / 2.0 ) );
			mast->AddComponent( slab );
			slab = new Slab( mast_width, 4.0 * mast_width, 6 * mast_width );
			slab->SetPosition( 0.0, - 2.5 * mast_width, - ( mast_height / 2.0 - 4.0 * mast_width ) );
			slab->SetColor( BLUE );
			mast->AddComponent( slab );

			for ( int i = 0; i < 11; i++ ) {
				target = new Sphere( 10.0 );
				target->SetColor( Translucid( RED ) );
				target->SetPosition( 0.0, mast_width, ( i + 1 ) * 50.0 - mast_height / 2.0  );
				mast->AddComponent( target );
			}
			mast->SetOffset( 0.0, 0.0, mast_height / 2.0 );
			mast->SetPosition( - 0.45 * tablet_width, - 0.0 * tablet_length, tablet_thickness );
			mast->SetColor( mast_color );
			AddComponent( mast );

			SetAttitude( 0.0, 90.0, 0.0 );
			SetOffset( tablet_width / 2.0, - tablet_thickness / 2.0, - tablet_length / 2.0 );

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

			Cylinder *cable = new Cylinder( 6.0, 6.0, 160.0 );
			cable->SetColor( 0.0, 0.2, 0.0 );
			cable->SetOrientation( 0.0, 90.0, 0.0 );
			AddComponent( cable );

			Cylinder *cone = new Cylinder( 30.0, 24.0, 50.0 );
			cone->SetOrientation( 0.0, 90.0, 0.0 );
			cone->SetColor( wrist_color );
			AddComponent( cone );

		}

	};

}

