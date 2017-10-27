///
/// Module:	GraspGLObjects
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		03 May 2016
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#include "stdafx.h"
#include "GraspGLObjects.h"

using namespace PsyPhy;
using namespace Grasp;

// Textures that are used to decorate certain objects.
// These files have to be in the execution directory.
const char *GraspGLObjects::wall_texture_bitmap = "Bmp\\Rockwall.bmp";
const char *GraspGLObjects::references_texture_bitmap = "Bmp\\Metal.bmp";
const char *GraspGLObjects::sky_texture_bitmap= "Bmp\\NightSky.bmp";

// These textures are the messages presented to the subject on a rotating disk.
const char *GraspGLObjects::ready_to_start_bitmap = "Bmp\\ReadyToStart.bmp";
const char *GraspGLObjects::block_completed_bitmap = "Bmp\\BlockCompleted.bmp";
const char *GraspGLObjects::lower_arm_bitmap = "Bmp\\LowerArm.bmp";
const char *GraspGLObjects::lower_arm_timeout_bitmap = "Bmp\\LowerArmTimeout.bmp";
const char *GraspGLObjects::raise_arm_bitmap = "Bmp\\RaiseArm.bmp";
const char *GraspGLObjects::raise_arm_timeout_bitmap = "Bmp\\RaiseArmTimeout.bmp";
const char *GraspGLObjects::head_misalign_bitmap = "Bmp\\HeadMisalignment.bmp";
const char *GraspGLObjects::head_align_timeout_bitmap = "Bmp\\HeadAlignmentTimeout.bmp";
const char *GraspGLObjects::response_timeout_bitmap = "Bmp\\ResponseTimeout.bmp";
const char *GraspGLObjects::timeout_bitmap = "Bmp\\TimeLimit.bmp";
const char *GraspGLObjects::hand_too_soon_bitmap = "Bmp\\HandTooSoon.bmp";
const char *GraspGLObjects::hand_should_not_bitmap = "Bmp\\HandShouldNot.bmp";
const char *GraspGLObjects::hand_rotate_timeout_bitmap = "Bmp\\HandRotateTimeout.bmp";
const char *GraspGLObjects::straighten_head_bitmap = "Bmp\\StraightenHead.bmp";
const char *GraspGLObjects::manual_reject_bitmap = "Bmp\\RejectTrial.bmp";
const char *GraspGLObjects::invalidate_trial_bitmap = "Bmp\\InvalidateTrial.bmp";
const char *GraspGLObjects::vr_completed_bitmap = "Bmp\\VRCompleted.bmp";
const char *GraspGLObjects::demo_bitmap = "Bmp\\DemoWorking.bmp";

// Dimensions of the room.
const double GraspGLObjects::room_radius = 1000.0;
const double GraspGLObjects::room_length = 6000.0;
const int GraspGLObjects::room_facets = 32;
const double GraspGLObjects::reference_bars = 5;
const double GraspGLObjects::reference_bar_radius = 50.0;
const double GraspGLObjects::reference_bar_facets = 8;

// Dimensions of objects in the room;

// Head shape is specified as the axis radii of an ellipsoid.
const Vector3 GraspGLObjects::head_shape = { 100.0, 150.0, 125.0 };
// Torso shape is a slab with width, height and thickness.
const Vector3 GraspGLObjects::torso_shape = { 200.0, 300.0, 125.0 };

// This is where spinning prompts go.
double GraspGLObjects::prompt_radius = 60.0;
Vector3 GraspGLObjects::prompt_location = { 0.0, 0.0, -750.0 };

// We may want to use a circular arrow to indicate to the subject which
//  way to tilt the head, in addition to color cues. This is where it is show wrt the eyes.
Vector3 GraspGLObjects::arrow_location = { 0.0, 0.0, -100.0 };
double GraspGLObjects::head_arrow_radius = 250.0;
double GraspGLObjects::hand_arrow_radius = 60.0;
double GraspGLObjects::outer_visor_radius = 320.0;
double GraspGLObjects::inner_visor_radius = 250.0;

Vector3 GraspGLObjects::desired_wrist_location = { 0.0, 0.0, -400.0 };
Vector3 GraspGLObjects::initial_hand_position = { 0.0, -500.0, 0.0 };

Vector3 GraspGLObjects::v_tool_location = { 0.0, 0.0, -500.0 };
double GraspGLObjects::v_tool_size = 2.0;
double GraspGLObjects::vk_tool_size = 1.0;

// The target is a line of spheres.
double GraspGLObjects::target_ball_radius = 100.0;
double GraspGLObjects::target_ball_spacing = 2.0 * room_radius / 7.5;
int GraspGLObjects::target_balls = 3;
// Or it may be a set of parallel lines.
int GraspGLObjects::target_bars = 10;								//Tagliabue
double GraspGLObjects::target_bar_radius = 20.0;					//Tagliabue
double GraspGLObjects::target_bar_spacing = 2.0 * room_radius / 20;	//Tagliabue
bool GraspGLObjects::useBars = true;

Vector3 GraspGLObjects::end_of_tunnel = { 0.0, 0.0, - room_length / 2.0 };
Vector3 GraspGLObjects::sky_location = { 0.0, 0.0, - room_length / 2.0 - 1000.0 };
// Visual target is placed a little bit closer to the subject than the end of the tunnel.
Vector3 GraspGLObjects::target_location = { 0.0, 0.0, - ( room_length / 2.0 - 500.0 ) };

double GraspGLObjects::finger_ball_radius = 10.0;
double GraspGLObjects::finger_length = 100.0;
double GraspGLObjects::laser_distance = room_length / 2.0 - room_radius - 500.0;

// Make things attached to the heads-up display (HUD) semi-transparent.
double GraspGLObjects::hmdTransparency = 0.5;

// A generic parameter that sets the number of facets in rounded objects.
// More facets means smoother surfaces, but slower drawing times;
int GraspGLObjects::curve_facets = 20;

// Transparency of objects that change color according to roll angle.
// It is important that the halo (glasses) be transparent. The kkTool will be transparent
//  too as a side effect, but that's not so important.
const double GraspGLObjects::errorColorMapTransparency = 0.5;
const double GraspGLObjects::errorColorMapFadeDistance = 7.5;

// Set up the lighting and material properties.
void GraspGLObjects::SetLighting( void ) {

	GLfloat fintensity = 0.75;
	GLfloat ambient = fintensity;
	GLfloat diffuse = fintensity;
	GLfloat specular = fintensity;

	// Light definition
	GLfloat LightPosition[] = { 0.0, 0.0, 1000.0, 0.0 };
	GLfloat LightAmbient[]  = { ambient, ambient, ambient, 1.0};
	GLfloat LightDiffuse[]  = { diffuse, diffuse, diffuse, 1.0};
	GLfloat LightSpecular[] = { specular, specular, specular, 1.0};

	glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
	glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, LightSpecular );

	glEnable( GL_LIGHT0 );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
	glEnable( GL_BLEND );

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
}

void GraspGLObjects::CreateTextures( void ) {

	sky_texture = new Texture( sky_texture_bitmap, 4000, 4000 );
	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	wall_texture = new Texture( wall_texture_bitmap, 1000, 2000 );
	// The cylindrical reference bars on the walls use a different texture.
	references_texture = new Texture( references_texture_bitmap, 500, 500 );

}

Assembly *GraspGLObjects::CreateStarrySky( void ) {
	Assembly *sky = new Assembly();
	Patch *patch = new Patch( room_radius * 4.4, room_radius * 4.4 );
	patch->SetTexture( sky_texture );
	sky->AddComponent( patch );
	sky->SetColor(WHITE);
	return( sky );
}

Assembly *GraspGLObjects::CreateDarkSky( void ) {
	Assembly *darkSky = new Assembly();
	Patch *patch = new Patch( room_radius * 2.2, room_radius * 2.2 );
	patch->SetTexture( sky_texture );// there is something strange if I dont put the texture the ligthing of the whole tunnel changes
	darkSky->AddComponent( patch );
	darkSky->SetColor( BLACK );
	darkSky->enabled = false;
	return( darkSky );
}

Assembly *GraspGLObjects::CreateRoom( void ) {

	Assembly *structure = new Assembly();
	structure->SetColor( BLACK );

	// Tunnel
	tunnel = new Cylinder( room_radius, room_radius, room_length, room_facets );
	tunnel->SetColor( WHITE );
	tunnel->SetTexture( wall_texture );
	tunnel->SetOrientation( 90.0, 0.0, 0.0 );
	structure->AddComponent( tunnel );

	// Reference Bars 
	double bar_length = room_length - 5.0 * reference_bar_radius;
	for (int i=0; i < reference_bars; i++ ){ 
		Cylinder *referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius, bar_length, reference_bar_facets );
		referenceBar->SetOffset( room_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  1.0 - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0f - (double) i / reference_bars, 1.0 );
		// The texturing on the bars may be commented out for the moment because it lengthens the rendering time too much.
		referenceBar->SetTexture( references_texture );
		structure->AddComponent( referenceBar );
		referenceBar = new Cylinder( reference_bar_radius, reference_bar_radius, bar_length, reference_bar_facets );
		referenceBar->SetOffset( room_radius, 0.0, 0.0 );
		referenceBar->SetOrientation( - 90.0 + 180 * (float) i / (float) reference_bars, referenceBar->kVector );
		referenceBar->SetColor(  (double) i / reference_bars, (double) i / reference_bars, (double) i / reference_bars, 1.0 );
		// See above.
		referenceBar->SetTexture( references_texture );
		structure->AddComponent( referenceBar );
	}

	Sphere *sphere = new Sphere( target_ball_radius );
	sphere->SetPosition( 0.0, 0.0, room_length / 2.0 );
	sphere->SetColor( RED );
	structure->AddComponent( sphere );

	return structure;
}

Assembly *GraspGLObjects::CreateOrientationTarget( void ) {

	Assembly *target = new Assembly();

	if ( useBars ) {
		for (int trg = - target_bars ; trg <= target_bars ; trg++ ){
			Cylinder *cylinder = new Cylinder( target_bar_radius, target_bar_radius,2.0 * room_radius );
			cylinder->SetPosition( 0.0 + target_bar_spacing * trg, 0.0, 0.0 );
			cylinder->SetOrientation(0.0, 90.0, 0.0);
			cylinder->SetColor( Translucid( BLUE ) );
			target->AddComponent( cylinder );
		}
	}
	else {
		for (int trg = - target_balls ; trg <= target_balls ; trg++ ){
			Sphere *sphere = new Sphere( target_ball_radius );
			sphere->SetPosition( 0.0, 0.0 + target_ball_spacing * trg, 0.0 );
			sphere->SetColor(( 255.0 - abs(trg) * 50.0 ) / 255.0, 0.0, 0.0);
			target->AddComponent( sphere );
		}
	}

	return target;
}

Assembly *GraspGLObjects::CreateResponse( void ) {
	static int fingers = target_balls - 1;
	static double finger_spacing = target_ball_radius;
	Assembly *projectiles = new Assembly();
	for ( int trg = - fingers; trg <= fingers ; trg++ ){
		Sphere *sphere = new Sphere( target_ball_radius * 0.75 );
		sphere->SetColor( 200.0f/255.0f, (75.0f + float(trg) * 75.0f/2.0f)/255.0f , 0.0f, 1.0f  );
		// Space the balls vertically.
		sphere->SetPosition( 0.0, 0.0 + target_ball_spacing * trg, 0.0 );
		projectiles->AddComponent( sphere );
	}
	return projectiles;
}

Assembly *GraspGLObjects::CreatePositionOnlyTarget( void ) {
	Assembly *target = new Assembly();
	Sphere *sphere = new Sphere( room_radius - 100.0 );
	target->AddComponent( sphere );
	target->SetColor( Translucid( CYAN ) );

	// I put back the sphere so that we don't get a flattened disk in an oblique view.
	// I changed the color to CYAN to provide a better background to the yellow-red hand.

	//Disk *disk = new Disk( room_radius - 80.0, 0.0 );//Tagliabue (disk instead of sphere otherwise it hides 
	//disk->SetPosition(0.0 , 0.0 , 5.0);//Tagliabue (moved slightly in front of the sky otherwhise interferences)
	//target->AddComponent( disk );//Tagliabue
	//target->SetColor( Translucid( ORANGE ) );//Tagliabue (translucid is to avoid to see too clearly the pixels)
	return target;
}

Glasses *GraspGLObjects::CreateGlasses( void ) {

	// Create a circular portal to look through to avoid visual cues about the egocentric reference frame.
	Glasses *glasses = new Glasses( inner_visor_radius, outer_visor_radius, room_radius, room_radius, 16 );
	return glasses;
}

// Need to change the hand to grey when outside the cone around the center line.
// But otherwise the fingers each have a different color. So we need to do some work
// to switch back and forth between grey and the defined colors of each finger.
void GraspGLObjects::SetHandColor( Assembly *hand, bool state ) {
	for ( int i = 0; i < hand->components; i++ ) {
		// Create a color that varies as a function of the finger's position and apply it to the entire finger.
		if ( state ) hand->component[i]->SetColor( 0.75f, 0.15f * (float) i, 0.0f, 1.0f  );
		else hand->component[i]->SetColor( 0.0, 0.0, 0.0, 0.85  );
	}
}

Assembly *GraspGLObjects::CreateVisualTool( double magnifier ) {

	Assembly *tool = new Assembly();

	// Create a set of 'fingers', each of which is a 'capsule' composed of a tube with rounded caps.
	static int n_fingers = target_balls-1;

	double radius = finger_ball_radius * magnifier;
	double length = finger_length * magnifier;

	double spacing = radius * 2.0;

	for ( int trg = - n_fingers; trg <= n_fingers ; trg++ ){


		// Each finger is a 'capsule' composed of a cylinder that is capped on each end with a sphere.
		Assembly *finger = new Assembly();
		Sphere *sphere = new Sphere( radius );
		sphere->SetPosition( 0.0, 0.0, 0.0 );
		finger->AddComponent( sphere );
		Cylinder *cylinder = new Cylinder( radius, radius, length );
		cylinder->SetPosition( 0.0, 0.0, - length / 2 );
		finger->AddComponent( cylinder );
		sphere = new Sphere( radius );
		sphere->SetPosition( 0.0, 0.0, - length );
		finger->AddComponent( sphere );

		// Space the fingers vertically.
		finger->SetPosition( 0.0, spacing * trg, 0.0 );
		tool->AddComponent( finger );
	}
	SetHandColor( tool, true );

	return tool;
}

Assembly *GraspGLObjects::CreateKinestheticTool( void ) {

	static const double hand_radius = finger_ball_radius * 2.5;
	Assembly *finger = new Assembly();
	Sphere *sphere = new Sphere( hand_radius );
	sphere->SetPosition( 0.0, 0.0, 0.0 );
	finger->AddComponent( sphere );
	Cylinder *cylinder = new Cylinder( hand_radius, hand_radius, finger_length );
	cylinder->SetPosition( 0.0, 0.0, - finger_length / 2 );
	finger->AddComponent( cylinder );
	sphere = new Sphere( hand_radius );
	sphere->SetPosition( 0.0, 0.0, - finger_length );
	finger->AddComponent( sphere );
	// Set a default color. I like blue.
	// In K-K color will be varied according to the orientation error.
	finger->SetColor( 0.0, 0.0, 1.0, 1.0 );
	return finger;

}

Assembly *GraspGLObjects::CreateLaserPointer( void ) {
	Assembly *laserPointer = new Assembly();
	Sphere *sphere = new Sphere( finger_ball_radius*2.0 );
	sphere->SetPosition( 0.0, 0.0, - laser_distance );
	laserPointer->AddComponent( sphere );
	// Laser is off by default.
	laserPointer->Disable();
	return laserPointer;
}

FuzzyPointer *GraspGLObjects::CreateFuzzyLaserPointer( void ) {
	FuzzyPointer *laserPointer = new FuzzyPointer();
	laserPointer->SetOffset( 0.0, 0.0, - laser_distance );
	laserPointer->SetColor( 1.0, 0.0, 1.0, 1.0 );
	// Laser is off by default.
	laserPointer->Disable();
	return laserPointer;
}

// Create a translucid cloud to show where to place the hand.
Assembly *GraspGLObjects::CreateZone( void ) {
	Assembly *assembly = new Assembly();
	Disk *disk = new Disk( 50.0, 0.0, 128 );
	assembly->AddComponent( disk );
	// Set the color to a blinking translucid green.
	assembly->SetColor( 0.0, 1.0, 0.0, - 0.05 );
	return assembly;
}

Assembly *GraspGLObjects::CreateRollPrompt( double radius ) {

	Assembly *prompt = new Assembly();

	// Angular extent of the circular arrow, where 1.0 = 360°.
	double guage =  radius / 10.0;
	double arc = 0.85;

	Annulus *donut = new Annulus( radius, guage, arc, curve_facets, curve_facets );
	donut->SetAttitude( 0.0, 90.0, 0.0 );
	prompt->AddComponent( donut );

	TaperedAnnulus *tip = new TaperedAnnulus( radius, radius / 3.0, 1.0, 0.05, curve_facets );
	tip->SetAttitude( 0.0, 90.0, 0.0 );
	tip->SetOrientation( - radius * 360.0, 0.0, 0.0 );
	prompt->AddComponent( tip );

	Ellipsoid *base = new Ellipsoid ( guage, guage / 2.0, guage );
	base->SetPosition( radius, 0.0, 0.0 );
	prompt->AddComponent( base );
	prompt->SetColor( 0.5, 0.5, 0.5, 0.5 );

	return prompt;

}

Assembly *GraspGLObjects::CreateSuccessIndicator( void ) {
	Assembly	*assembly = new Assembly();
	Sphere		*sphere = new Sphere( prompt_radius );
	sphere->SetColor( GREEN );
	assembly->AddComponent( sphere );
	return assembly;
}

Assembly *GraspGLObjects::CreateIndicator( Texture *texture, double hole_radius ) {

	Assembly	*assembly = new Assembly();
	Disk		*surface;

	surface = new Disk( 120.0, hole_radius, 128 );
	surface->SetTexture( texture );
	assembly->AddComponent( surface );
	assembly->SetColor( 0.9, 0.9, 1.0, hmdTransparency );
	assembly->Disable();

	return assembly;
}


// Group all the elements that move with the hand into a single entity.
// Each component will be activated or deactivated separately, but their pose will be set
// jointly by accessing the hand object.
Yoke *GraspGLObjects::CreateHand( void ) {
	Yoke *hand = new Yoke();
	hand->AddComponent( vkTool );
	hand->AddComponent( kTool );
	hand->AddComponent( kkTool );
	hand->AddComponent( handLaser );
	lowerHandPrompt = new Sphere( finger_length );
	// Set the color to a transparent blinking red.
	lowerHandPrompt->SetColor( 1.0, 0.0, 0.0, -0.02 );
	lowerHandPrompt->Disable();
	hand->AddComponent( lowerHandPrompt );
	return( hand );
}

Yoke *GraspGLObjects::CreateHUD( void ) {

	Yoke *yoke = new Yoke();

	// The glasses will be positioned at the same place as the head based on the tracker.
	// By setting the offset in depth, the glasses will be positioned a bit in front of the subject.
	glasses->SetOffset( 0.0, 0.0, -400.0 );
	yoke->AddComponent( glasses );

	// An arrow that shows which way to tilt the head. It also moves as if in a heads up display.
	headTiltPrompt->SetOffset( arrow_location );
	yoke->AddComponent( headTiltPrompt );

	// Similarly, the laser pointer attached to the head moves with it.
	gazeLaser = CreateLaserPointer();
	gazeLaser->SetColor( 0.7, 0.0, 0.4, 1.0 );
	yoke->AddComponent( gazeLaser );

	// When responding in the V only mode, the virtual tool (hand) moves with the gaze.
	vTool->SetOffset( v_tool_location );
	yoke->AddComponent( vTool );

	// Round signs conveying messages to the subject also move with the head.
	spinners = new Assembly();

	head_misalign_texture = new Texture( head_misalign_bitmap );
	headMisalignIndicator = CreateIndicator( head_misalign_texture );
	spinners->AddComponent( headMisalignIndicator );

	manual_reject_texture = new Texture( manual_reject_bitmap );
	manualRejectIndicator = CreateIndicator( manual_reject_texture );
	spinners->AddComponent( manualRejectIndicator );

	invalidate_trial_texture = new Texture( invalidate_trial_bitmap );
	invalidateTrialIndicator = CreateIndicator( invalidate_trial_texture );
	spinners->AddComponent( invalidateTrialIndicator );

	head_align_timeout_texture = new Texture( head_align_timeout_bitmap );
	headAlignTimeoutIndicator = CreateIndicator( head_align_timeout_texture );
	spinners->AddComponent( headAlignTimeoutIndicator );
	headTiltTimeoutIndicator = CreateIndicator( head_align_timeout_texture );
	spinners->AddComponent( headTiltTimeoutIndicator );

	raise_arm_timeout_texture = new Texture( raise_arm_timeout_bitmap );
	raiseArmTimeoutIndicator= CreateIndicator( raise_arm_timeout_texture );
	spinners->AddComponent( raiseArmTimeoutIndicator );

	lower_arm_texture = new Texture( lower_arm_bitmap );
	lowerHandIndicator = CreateIndicator( lower_arm_texture );
	spinners->AddComponent( lowerHandIndicator );

	lower_arm_timeout_texture = new Texture(lower_arm_timeout_bitmap );
	lowerArmTimeoutIndicator = CreateIndicator( lower_arm_timeout_texture );
	spinners->AddComponent( lowerArmTimeoutIndicator );

	ready_to_start_texture = new Texture( ready_to_start_bitmap );
	readyToStartIndicator = CreateIndicator( ready_to_start_texture );
	spinners->AddComponent( readyToStartIndicator );

	block_completed_texture = new Texture( block_completed_bitmap );
	blockCompletedIndicator = CreateIndicator( block_completed_texture );
	spinners->AddComponent( blockCompletedIndicator );

	vr_completed_texture = new Texture( vr_completed_bitmap );
	vrCompletedIndicator = CreateIndicator( vr_completed_texture );
	spinners->AddComponent( vrCompletedIndicator );

	demo_texture = new Texture( demo_bitmap );
	demoIndicator = CreateIndicator( demo_texture );
	spinners->AddComponent( demoIndicator );

	timeout_texture = new Texture( timeout_bitmap );
	timeoutIndicator = CreateIndicator( timeout_texture );
	spinners->AddComponent( timeoutIndicator );

	response_timeout_texture = new Texture( response_timeout_bitmap );
	responseTimeoutIndicator = CreateIndicator( response_timeout_texture );
	spinners->AddComponent( responseTimeoutIndicator );

	hand_too_soon_texture = new Texture( hand_too_soon_bitmap );
	handTooSoonIndicator = CreateIndicator( hand_too_soon_texture );
	spinners->AddComponent( handTooSoonIndicator );

	hand_rotate_timeout_texture = new Texture( hand_rotate_timeout_bitmap );
	handRotateTimeoutIndicator = CreateIndicator( hand_rotate_timeout_texture );
	spinners->AddComponent( handRotateTimeoutIndicator );

	hand_should_not_texture = new Texture( hand_should_not_bitmap );
	handShouldNotBeRaisedIndicator = CreateIndicator( hand_should_not_texture );
	spinners->AddComponent( handShouldNotBeRaisedIndicator );

	straighten_head_texture = new Texture( straighten_head_bitmap );
	straightenHeadIndicator = CreateIndicator( straighten_head_texture );
	gazeStraightAheadIndicator = new Disk( 30.0 );
	gazeStraightAheadIndicator->SetColor( GREEN );
	//straightenHeadIndicator->AddComponent( gazeStraightAheadIndicator );

	spinners->AddComponent( straightenHeadIndicator );

	raise_arm_texture = new Texture( raise_arm_bitmap );
	raiseHandIndicator = CreateIndicator( raise_arm_texture, 1.0 );
	// Set this indicator to be green and very transparent.
	// raiseHandIndicator->SetColor( 0.0, 1.0, 0.0, 0.05 );
	spinners->AddComponent( raiseHandIndicator );

	spinners->SetOffset( prompt_location );
	yoke->AddComponent( spinners );

	return( yoke );

}

Assembly *GraspGLObjects::CreateProjectiles( int fingers ) {

	Assembly *assembly = new Assembly();
	double finger_spacing = finger_ball_radius * 2;

	for ( int trg = - fingers; trg <= fingers ; trg++ ){
		Sphere *sphere = new Sphere( finger_ball_radius*1.0 );
		// Create a color that varies as a function of the ball's position.
		if ( fingers == 0 ) sphere->SetColor( 200.0f/255.0f, 0.0f, 0.0f, 1.0f );
		else sphere->SetColor( 200.0f/255.0f, (75.0f + float(trg) * 75.0f / 2.0f ) / 255.0f , 0.0f, 1.0f );
		// Space the balls vertically.
		sphere->SetPosition( 0.0, finger_spacing * trg, 0.0 );
		assembly->AddComponent( sphere );
	}

	return assembly;

}

void GraspGLObjects::CreateVRObjects( void ) {

	CreateTextures();

	room = CreateRoom();
	glasses = CreateGlasses();
	glasses->SetColor( 0.5, 0.5, 0.5, 0.5 );
	starrySky = CreateStarrySky();
	darkSky = CreateDarkSky();
	// Only one sky should be enabled at a time, so we disable darkSky at this point.
	//darkSky->Disable();
	// The skies get attached to the room so that they move (tilt) with it.
	// This means that you don't actually have to draw the skies explicitly.
	room->AddComponent( starrySky );
	room->AddComponent( darkSky );

	orientationTarget = CreateOrientationTarget();
	positionOnlyTarget = CreatePositionOnlyTarget();
	straightAheadTarget =  CreatePositionOnlyTarget();

	response = CreateResponse();
	successIndicator = CreateSuccessIndicator();

	// Attach these objects to the room so that they move when conflict is applied.
	room->AddComponent( orientationTarget );
	room->AddComponent( positionOnlyTarget );
	room->AddComponent( response );
	room->AddComponent( successIndicator );

	headTiltPrompt = CreateRollPrompt( head_arrow_radius );
	headTiltPrompt->SetColor( 0.5, 0.0, 0.4 );

	handRollPrompt = CreateRollPrompt( hand_arrow_radius );
	handRollPrompt->SetColor( 0.0, 0.0, 0.4 );
	handRollPrompt->SetOffset( 0.0, 0.0, - finger_length / 2.0 );


	projectiles = new Assembly;
	multiProjectile = CreateProjectiles( target_balls - 1 );
	multiProjectile->Disable();
	projectiles->AddComponent( multiProjectile );
	monoProjectile = CreateProjectiles( 0 );
	projectiles->AddComponent( monoProjectile );
	monoProjectile->Disable();

	wristZone = CreateZone();

	// Orientated tool used when responding with only visual feedback (e.g. V-V).
	vTool = CreateVisualTool( v_tool_size );
	// Orientated tool that allows visual feedback of the hand's orientation.
	vkTool = CreateVisualTool( vk_tool_size );
	// A tool that allows one to point, but does not show the roll orientation.
	kTool = CreateKinestheticTool();
	// Same as the above, but this one we use when presenting a kinesthetic target in K-K.
	kkTool = CreateKinestheticTool();
	// A laser attached to the hand.
	handLaser = CreateFuzzyLaserPointer();
	handLaser->SetColor( 1.0, 0.0, 1.0, 1.0 );
	// Collect all the things that may be attached to the hand.
	hand = CreateHand();
	// And to the gaze.
	hmd = CreateHUD();
	// By default, the tool used to shoot the projectiles is the one at the hand.
	selectedTool = hand;
}

// Place objects at default locations.
void GraspGLObjects::PlaceVRObjects( void ) {
	starrySky->SetPosition( sky_location );
	darkSky->SetPosition( sky_location );
	successIndicator->SetPosition( prompt_location );
	orientationTarget->SetPosition( target_location );
	positionOnlyTarget->SetPosition( end_of_tunnel );
	straightAheadTarget->SetPosition( end_of_tunnel );
	// Place the stationary object that shows the subject's response a little bit in front of the targets.
	response->SetPosition( target_location[X], target_location[Y], target_location[Z] + target_ball_radius * 2.0 );
	wristZone->SetPosition( desired_wrist_location );
	// Initially set the hand position away from zero. This will be immediately overridden by 
	// a valid tracker reading. But if the hand is not visible to the tracker, this should 
	// ensure that the hand is not at the position of the eyes.
	hand->SetPosition( initial_hand_position );
}

// Modulate the color of an object according to it's roll angle wrt a specified desired roll angle.
void GraspGLObjects::SetColorByRollError( OpenGLObject *object, double roll_error, double sweet_zone, double opacity ) {

	double magnitude = fabs( roll_error );

	if ( magnitude < sweet_zone ){
		// If we are within the sweet zone, the color is bright green.
		// This makes it easier for the subject to recognize when the head is properly aligned.
		// But one need not be in the sweet zone for the orientation to be considered good.
		// If the orientation is within tolerance, it is good as well. This means that the color
		// starts to change before leaving the tolerance zone (assuming that tolerance > sweet ).
		object->SetColor( 0.0, 1.0,  0.0, opacity );
	} else {
		// Colors will change as one moves farther from the center.
		if ( ( magnitude - sweet_zone ) < errorColorMapFadeDistance ) {
			// Go from cyan at the edge of the sweet zone to yellow at the limit of the tolerance zone.
			double fade = ( magnitude - sweet_zone ) / errorColorMapFadeDistance;
			object->SetColor( fade, 1.0, 1.0 - fade, opacity );
		}
		else if ( ( magnitude - sweet_zone ) < 2.0 * errorColorMapFadeDistance ) {
			// As we get even farther away, fade from yellow to red.
			double fade = ( magnitude - sweet_zone - errorColorMapFadeDistance ) / errorColorMapFadeDistance;
			object->SetColor( 1.0, 1.0 - fade, 0.0, opacity );
		}
		// If more than 2 epsilons, the color is red.
		else object->SetColor( 1.0, 0.0, 0.0, opacity );
	}
}

// Draw the objects that are used during VR rendering.
// Note that only those objects that are currently active are actually drawn.
void GraspGLObjects::DrawVR( void ) {

	// Draw the room with non-shiny material.
	glUsefulMatteMaterial();

	// Because the skies are attached to the room, one need not draw them explicitly.
	// I leave these lines here in comments, though, to remind us that they will be drawn.
	// If we ever decide not to attach them to the room, these lines should be uncommented.
	// DrawLightSky();
	// DrawDarkSky();

	// Things to be drawn in matte finish (no specular).
	room->Draw();
	glasses->Draw();
	gazeLaser->Draw();
	straightAheadTarget->Draw();

	// Draw the other objects with the hopes of seeing specular reflections. 
	glUsefulShinyMaterial();

	vTool->Draw();
	vkTool->Draw();
	kTool->Draw();
	kkTool->Draw();
	projectiles->Draw();

	// Draw some other objects in matte.
	glUsefulMatteMaterial();
	handLaser->Draw();

	headTiltPrompt->Draw();
	handRollPrompt->Draw();
	spinners->Draw();
	wristZone->Draw();
	lowerHandPrompt->Draw();

	// The following are now attached to the room, so they get drawn with the room, if activated.
	// orientationTarget->Draw();
	//response->Draw();
	//successIndicator->Draw();
	//raiseHandIndicator->Draw();
	//positionOnlyTarget->Draw();

}

// The following objects are not used during the Grasp protocol and are not seen by the subject.
// Rather, these objects are used in the GraspGUI and elswhere to visualize the subject's pose.
// Perhaps they would be better placed in another class so that they are not included when not needed.
Assembly *GraspGLObjects::CreateHead( void ) {

	Assembly *head = new Assembly();
	// Skull
	Ellipsoid *skull = new Ellipsoid( head_shape );
	skull->SetColor( .4f, 0.0f, .4f );
	// Eyes
	head->AddComponent( skull );
	Sphere *sphere = new Sphere( 20.0 );
	sphere->SetColor( 1.0f, 0.0f, .5f );
	sphere->SetPosition( -50.0, 20.0, -100.0 );
	head->AddComponent( sphere );
	sphere = new Sphere( 20.0 );
	sphere->SetColor( 1.0f, 0.0f, .5f );
	sphere->SetPosition( 50.0, 20.0, -100.0 );
	head->AddComponent( sphere );
	// Nose
	Cylinder *cylinder = new Cylinder( 20.0, 5.0, 30.0 );	
	cylinder->SetPosition( 0.0, -20.0, -100.0 );
	cylinder->SetOrientation( 0.0, 90.0, 0.0 );
	cylinder->SetColor( YELLOW );
	head->AddComponent( cylinder );
	return head;
}

Assembly *GraspGLObjects::CreateTorso( void ) {

	Assembly *torso = new Assembly();
	Slab *slab = new Slab( torso_shape[X], torso_shape[Y], torso_shape[Z] );
	slab->SetColor( 0.1f, 0.4f, 0.0f );
	torso->AddComponent( slab );
	Disk *disk = new Disk( 50.0 );
	disk->SetPosition( 0.0, 0.0, -40.0 );
	disk->SetColor( 1.0f, 0.7f, 0.0f );
	torso->AddComponent( disk );
	return torso;

}


void GraspGLObjects::CreateAuxiliaryObjects( void ) {
	head = CreateHead();
	torso = CreateTorso();

	hmdStructure = CreateHmdMarkerStructure( "Bdy\\CADHMD.bdy" );
	hmdStructure->SetPosition( 0.0, 0.0, -500.0 );

	handStructure = CreateHandMarkerStructure( "Bdy\\CADHand.bdy" );
	handStructure->SetPosition( 0.0, 0.0, -500.0 );

	chestStructure = CreateChestMarkerStructure( "Bdy\\CADChest.bdy" );
	chestStructure->SetPosition( 0.0, 0.0, -500.0 );

}

void GraspGLObjects::DrawHead( TrackerPose *pose ) {
	if ( pose != nullptr ) {
		head->SetPosition( pose->pose.position );
		head->SetOrientation( pose->pose.orientation );
	}
	head->Draw();
}

void GraspGLObjects::DrawTorso(  TrackerPose *pose  ) {
	if ( pose != nullptr ) {
		torso->SetPosition( pose->pose.position );
		torso->SetOrientation( pose->pose.orientation );
	}
	torso->Draw();
}

void GraspGLObjects::DrawBody( TrackerPose *pose ) {
	Vector3 torso_position;
	CopyVector( torso_position, pose->pose.position );
	torso_position[Y] -= (head_shape[Y] + torso_shape[Y] / 2.0);
	torso->SetPosition( torso_position );
	torso->SetOrientation( nullQuaternion );
	torso->Draw();
	DrawHead( pose );
}

#define STRUCTURE_BALL_RADIUS 15.0
#define STRUCTURE_BAR_RADIUS 15.0

void MarkerStructureGLObject::ShowVisibility( MarkerFrame &marker_frame, int led_on_color ) {
	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		if ( marker_frame.marker[ modelMarker[mrk].id ].visibility ) component[mrk]->SetColor( led_on_color );
		else component[mrk]->SetColor( BLACK );
	}
}

void MarkerStructureGLObject::AddBar( int marker1, int marker2 ) {

	Slab *slab;
	Vector3 center, delta;

	AddVectors( center, modelMarker[marker1].position, modelMarker[marker2].position );
	ScaleVector( center, center, 0.5 );
	SubtractVectors( delta, modelMarker[marker1].position, modelMarker[marker2].position );
	slab = new Slab( STRUCTURE_BAR_RADIUS, VectorNorm( delta ) * 1.1, STRUCTURE_BAR_RADIUS );
	slab->SetPosition( center[X], center[Y], center[Z] + STRUCTURE_BAR_RADIUS );
	slab->SetOrientation( ToDegrees( atan2( delta[X], delta[Y] ) ), 0.0, 0.0 );
	slab->SetColor( 1.0, 1.0, 0.0, 0.35 );
	AddComponent( slab );
}

MarkerStructureGLObject::MarkerStructureGLObject( char *model_file ) {

	if ( model_file ) {
		FILE *fp = fopen( model_file, "r" );
		fAbortMessageOnCondition( ( NULL == fp ), "ReadModelMarkerPositions()", "Error opening %s for read.", model_file );
		int mrk;
		for ( mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
			int id, items;
			double x, y, z;
			items = fscanf( fp, "%d %lf\t %lf\t %lf", &id, &x, &y, &z );
			if ( items != 4  ) break;
			fAbortMessageOnCondition( ( id < 0 || id >= MAX_MARKERS ), "ReadModelMarkerPositions()", "Marker %d ID = %d out of range [0 %d].", mrk, id, MAX_MARKERS - 1 );
			modelMarker[mrk].id = id;
			modelMarker[mrk].position[X] = x;
			modelMarker[mrk].position[Y] = y;
			modelMarker[mrk].position[Z] = z;
		}
		nModelMarkers = mrk;	
		fclose( fp );
	}
	else nModelMarkers = 0;

	for ( int mrk = 0; mrk < nModelMarkers; mrk++ ) {
		Sphere *sphere = new Sphere( STRUCTURE_BALL_RADIUS );
		sphere->SetPosition( modelMarker[mrk].position );
		sphere->SetColor( 0.0, 1.0, 0.0, 1.0 );
		AddComponent( sphere );
	}

}

MarkerStructureGLObject *GraspGLObjects::CreateHmdMarkerStructure ( char *model_file ) {

	MarkerStructureGLObject *structure = new MarkerStructureGLObject( model_file );

	Slab *bar;
	Assembly *frame = new Assembly();

	bar = new Slab( STRUCTURE_BAR_RADIUS, 100.0, STRUCTURE_BAR_RADIUS );
	bar->SetPosition(   0.0, 0.0, 0.0 );
	frame->AddComponent( bar );
	bar = new Slab( 210.0, STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS );
	bar->SetPosition(   0.0, 0.0, 0.0 );
	frame->AddComponent( bar );
	bar = new Slab( STRUCTURE_BAR_RADIUS, 160.0, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( 110.0, 0.0, 40.0 );
	frame->AddComponent( bar );
	bar = new Slab( STRUCTURE_BAR_RADIUS, 160.0, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( -110.0, 0.0, 40.0 );
	frame->AddComponent( bar );
	bar = new Slab( STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS, 40 );
	bar->SetPosition( -110.0, 0.0, 20.0 );
	frame->AddComponent( bar );
	bar = new Slab( STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS, 40 );
	bar->SetPosition( 110.0, 0.0, 20.0 );
	frame->AddComponent( bar );


	frame->SetPosition( 0.0, 0.0, -10.0 );

	structure->AddComponent( frame );


	structure->SetColor( Translucid( Translucid( GRAY ) ) );
	//	structure->SetOrientation( 0.0, 0.0, 90.0 );
	return( structure );
}

MarkerStructureGLObject *GraspGLObjects::CreateHandMarkerStructure ( char *model_file ) {

	Slab *bar;
	Assembly *frame, *square, *diamond;

	MarkerStructureGLObject *structure = new MarkerStructureGLObject( model_file );

	frame = new Assembly();

	square = new Assembly();
	bar = new Slab( STRUCTURE_BAR_RADIUS, 160.0, STRUCTURE_BAR_RADIUS );
	bar->SetPosition(   70.0, 0.0, 0.0 );
	square->AddComponent( bar );
	bar = new Slab( STRUCTURE_BAR_RADIUS, 160.0, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( - 70.0, 0.0, 0.0 );
	square->AddComponent( bar );
	bar = new Slab( 160.0, STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( 0.0, 70.0, 0.0 );
	square->AddComponent( bar );
	bar = new Slab( 160.0, STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( 0.0, -70.0, 0.0 );
	square->AddComponent( bar );

	frame->AddComponent( square );

	diamond = new Assembly();
	bar = new Slab( STRUCTURE_BAR_RADIUS, 80.0, STRUCTURE_BAR_RADIUS );
	bar->SetPosition(   35.0, 0.0, 0.0 );
	diamond->AddComponent( bar );
	bar = new Slab( STRUCTURE_BAR_RADIUS, 80.0, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( - 35.0, 0.0, 0.0 );
	diamond->AddComponent( bar );
	bar = new Slab( 80.0, STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( 0.0, 35.0, 0.0 );
	diamond->AddComponent( bar );
	bar = new Slab( 80.0, STRUCTURE_BAR_RADIUS, STRUCTURE_BAR_RADIUS );
	bar->SetPosition( 0.0, -35.0, 0.0 );
	diamond->AddComponent( bar );
	diamond->SetOrientation( 45.0, 0.0, 0.0 );
	diamond->SetPosition( 0.0, 0.0, -100.0 );

	frame->AddComponent( diamond );

	static double vertices[][2] = {{-100, -40}, {-100, 40}, {0, 70}, {100, 50}, {100, -50}, {0, -70}, {-100, -40}};
	Extrusion *plate = new Extrusion( 10.0, vertices, 7 );
	plate->SetOrientation( 0.0, 0.0, 90.0 );
	plate->SetPosition( -10.0, 0.0, 0.0 );
	frame->AddComponent( plate );

	square->SetColor( Translucid( GRAY ) );
	diamond->SetColor( Translucid( GRAY ) );
	plate->SetColor( Translucid( GRAY ) );
	frame->SetOffset( 0.0, 0.0, 70.0 );

	structure->AddComponent( frame );
	return( structure );
}

MarkerStructureGLObject *GraspGLObjects::CreateChestMarkerStructure ( char *model_file ) {
	MarkerStructureGLObject *structure = new MarkerStructureGLObject( model_file );
	double vertices[8][2];

	vertices[0][X] = -130.0;
	vertices[0][Y] = -90.0;
	vertices[1][X] = -130.0;
	vertices[1][Y] = 90.0;
	vertices[2][X] = 130.0;
	vertices[2][Y] = 90.0;
	vertices[3][X] = 130.0;
	vertices[3][Y] = -90.0;
	Extrusion *plate = new Extrusion( STRUCTURE_BAR_RADIUS, vertices, 4 );
	plate->SetColor( 0.5, 0.5, 0.5, 0.25 );
	plate->SetPosition( 0.0, 0.0, 60.0 );
	structure->AddComponent( plate );

	vertices[0][X] = 0.0;
	vertices[0][Y] = 60.0;
	vertices[1][X] = 80.0;
	vertices[1][Y] = 0.0;
	vertices[2][X] = 0.0;
	vertices[2][Y] = - 60.0;
	vertices[3][X] = - 80.0;
	vertices[3][Y] = 0.0;
	plate = new Extrusion( STRUCTURE_BAR_RADIUS, vertices, 4 );
	plate->SetPosition( 0.0, 0.0,  0.0 );
	plate->SetColor(  0.25, 0.25, 0.25, 0.25  );
	structure->AddComponent( plate );

	return( structure );
}

Glasses::Glasses( double inner_radius, double outer_radius, double width, double height, int facets ) {

	// Create a black mask with a circular hole to look through.
	// The idea is that it should be big enough to mask out to the edges of the visual scene.
	blinders = new Hole( outer_radius, width, height, facets );
	blinders->SetColor( BLACK );
	AddComponent( blinders );
	// Now create a frame around the portal. In the Grasp experiment, this frame can change colors
	//  in order to reflect the orientation of the head. I also think that having concentric circular
	//  borders, rather than a single black edge, further reduces the saliencey of the non-circular HMD viewing frustrum.
	frame = new Hole( inner_radius, width, height, facets );
	frame->SetOffset( 0.0, 0.0, - 20.0 );
	AddComponent( frame );
}

// Override SetColor so that the blinders are always black.
void Glasses::SetColor( float r, float g, float b, float a = 1.0 ) {
	Assembly::SetColor( r, g, b, a );
	blinders->SetColor( BLACK );
}


Vector3 FuzzyPointer::beamOffset[LASER_BEAMS] = {
	{  0.2,  0.5, 1.0 },
	{ -0.4,  0.9, 2.0 },
	{ -0.8, -0.3, 3.0 },
	{  0.3, -0.9, 4.0 },
	{  0.6,  0.1, 5.0 },
	{ -0.3,  0.2, 6.0 },
	{ -0.2, -0.5, 7.0 }
};

FuzzyPointer::FuzzyPointer() {

	for ( int beam = 0; beam < LASER_BEAMS; beam++ ) {
		sphere[beam] = new Disk( LASER_FOCUSED_SIZE );
		AddComponent( sphere[beam] );
	}
}

void FuzzyPointer::SetColor( float r, float g, float b, float a ) {
	a = 0.25f;
	Assembly::SetColor( r, g, b, a );
}
void FuzzyPointer::SetEccentricity( double projection ) {
	for ( int beam = 0; beam < LASER_BEAMS; beam++ ) {
		if ( projection > LASER_CLOSE_ALIGNMENT_THRESHOLD ) {
			sphere[beam]->SetOffset( 0.0, 0.0, (double) beam );
			sphere[beam]->SetRadius( LASER_FOCUSED_SIZE );
		}
		else {
			Vector3 offset;
			double distance = 2000.0 * (LASER_CLOSE_ALIGNMENT_THRESHOLD - projection);
			double expand = LASER_DIFFUSION_CONSTANT * distance;
			ScaleVector( offset, beamOffset[beam], 1.25 * expand );
			sphere[beam]->SetOffset( offset );
			sphere[beam]->SetRadius( LASER_FOCUSED_SIZE + expand );
		}
	}
}

void FuzzyPointer::Draw( void ) {
	for ( int beam = 0; beam < LASER_BEAMS; beam++ ) {
		double angle = 0.1 * (double) (beam + 7) * (double) GetTickCount() / ((double) (beam + 3.0 ) * 2.0) 
			+ 60.0 * sin( (double) GetTickCount() / 50.0 / (double) (beam + 7) );
		sphere[beam]->SetOrientation( angle, 0.0, 0.0 );
	}
	Assembly::Draw();
}


