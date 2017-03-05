///
/// Module:	GraspGLObjects
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		03 May 2016
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#pragma once
#include <gl/gl.h>
#include <gl/glu.h>

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLYoke.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/PoseTrackers.h"

namespace Grasp {

	using namespace PsyPhy;

#define LASER_BEAMS 7
#define LASER_FOCUSED_SIZE 20.0
#define LASER_DIFFUSION_CONSTANT 1.0
#define LASER_CLOSE_ALIGNMENT_THRESHOLD 0.99

	class FuzzyPointer : public PsyPhy::Assembly {
	public:
		Disk *sphere[LASER_BEAMS];
		static Vector3 beamOffset[LASER_BEAMS];
		void SetColor( float r, float g, float b, float a = 1.0f );
		void SetEccentricity( double projection );
		void Draw( void );
		FuzzyPointer();
	};

	class Glasses : public PsyPhy::Assembly {

	public:

		Hole	*frame;
		Hole	*blinders;

		Glasses( double inner_radius, double outer_radius, double width, double height, int facets );
		~Glasses( void );
		// Override SetColor so that the blinders are always black.
		void SetColor( float r, float g, float b, float a );

	};

	class MarkerStructureGLObject : public PsyPhy::Assembly {

	public:

		// A buffer that holds the model for the rigid body.
		// Each element is the number and the 3D position of a marker with respect
		//  to the control point of the rigid body when in the null pose.
		struct {
			int		id;
			Vector3	position;
		} modelMarker[MAX_MARKERS];
		int nModelMarkers;

	public:
		MarkerStructureGLObject( char *model_filename = NULL );
		~MarkerStructureGLObject( void ){}

		void AddBar( int marker1, int marker2 );
		void ShowVisibility( MarkerFrame &marker_frame );
	};

	class GraspGLObjects : public VectorsMixin {

	public:


		// Dimensions of the room.
		static const double room_radius;
		static const double room_length;
		static const int room_facets;
		static const double reference_bars;
		static const double reference_bar_radius;
		static const double reference_bar_facets;

		// Dimensions of objects in the room.
		static const Vector3 head_shape;
		static const Vector3 torso_shape;

		static const Vector3 target_location;
		static const Vector3 sky_location;
		static const Vector3 prompt_location;
		static const double prompt_radius;
		static double inner_visor_radius;
		static double outer_visor_radius;
		static Vector3 desired_wrist_location;
		static Vector3 initial_hand_position;

		static const double target_ball_radius;
		static const double finger_ball_radius;
		static const double target_ball_spacing;
		static const int target_balls;
		static const double finger_length;

		static const double hmdTransparency;

		static const double errorColorMapTransparency;
		static const double errorColorMapFadeDistance;

		static int GraspGLObjects::curve_facets;

	protected:

		// Textures that are used to decorate certain objects.
		// These files have to be in the execution directory.
		Texture			*wall_texture;				// The texture that is applied to the walls.
		static const char *wall_texture_bitmap;

		Texture			*references_texture;		// The bars that run along the tunnel walls are textured as well.
		static const char *references_texture_bitmap;

		Texture			*sky_texture;				// A starry sky.
		static const char *sky_texture_bitmap;

		Texture			*hand_icon_texture;			// A pictogram of a hand;
		static const char *hand_icon_bitmap;

		// Messages that are placed on a moving disk to give feedback to the subject in a way that avoids
		// establishing a reference frame based on the orientation of the text.

	public:

		Assembly		*spinners;

		Assembly		*readyToStartIndicator;			// Prompt the subject to press a button when ready.
		Texture			*ready_to_start_texture;		
		static const char *ready_to_start_bitmap;

		Assembly		*blockCompletedIndicator;		// Prompt the subject to take off the HMD and press 'Return' on the keyboard.
		Texture			*block_completed_texture;		
		static const char *block_completed_bitmap;

		Assembly		*lowerHandIndicator;				// Prompt the subject to raise and lower the hand appropriately.
		Texture			*lower_arm_texture;		
		static const char *lower_arm_bitmap;	 

		Assembly		*lowerArmTimeoutIndicator;
		Texture			*lower_arm_timeout_texture;		
		static const char *lower_arm_timeout_bitmap;	

		Assembly		*raiseHandIndicator;			
		Texture			*raise_arm_texture;		
		static const char *raise_arm_bitmap;	 

		Assembly		*raiseArmTimeoutIndicator;
		Texture			*raise_arm_timeout_texture;		
		static const char *raise_arm_timeout_bitmap;	

		Assembly		*headMisalignIndicator;			// Shown when the subject does not maintain the desired head tilt.
		Texture			*head_misalign_texture;		
		static const char *head_misalign_bitmap;

		Assembly		*manualRejectIndicator;			// Shown when the subject does not maintain the desired head tilt.
		Texture			*manual_reject_texture;		
		static const char *manual_reject_bitmap;

		Assembly		*headAlignTimeoutIndicator;
		Assembly		*headTiltTimeoutIndicator;
		Texture			*head_align_timeout_texture;		
		static const char *head_align_timeout_bitmap;

		Assembly		*timeoutIndicator;				// Shown when the subject takes too long to do something.
		Texture			*timeout_texture;			
		static const char *timeout_bitmap;	 

		Assembly		*responseTimeoutIndicator;		// Shown when the subject takes too long to confirm a response.
		Texture			*response_timeout_texture;		
		static const char *response_timeout_bitmap;	 

		Assembly		*handRotateTimeoutIndicator;	// Shown when the subject takes too long to rotat the hand to the desired orientation.
		Texture			*hand_rotate_timeout_texture;		
		static const char *hand_rotate_timeout_bitmap;	 

		Assembly		*handTooSoonIndicator;			// Shown when the subject raises the hand too soon.
		Texture			*hand_too_soon_texture;		
		static const char *hand_too_soon_bitmap;	 

		Assembly		*handShouldNotBeRaisedIndicator;	// Shown when the hand is raised and should not be.
		Texture			*hand_should_not_texture;		
		static const char *hand_should_not_bitmap;	 

		Assembly		*straightenHeadIndicator;	// Shown when the subject should straighent the hand on the shoulders.
		Disk			*gazeStraightAheadIndicator;
		Texture			*straighten_head_texture;		
		static const char *straighten_head_bitmap;	 

		Assembly		*vrCompletedIndicator;	// Shown in a special case to indicate that the session is over.
		Texture			*vr_completed_texture;		
		static const char *vr_completed_bitmap;	 

		Assembly		*demoIndicator;	// Shown when the subject should straighent the hand on the shoulders.
		Texture			*demo_texture;		
		static const char *demo_bitmap;	 

	public:

		Assembly		*orientationTarget;		// Shows the target orientation.
		Assembly		*positionOnlyTarget;	// Shows the center of the target location. Allows subject to aim without showing the target orientation.
		Assembly		*straightAheadTarget;	// Placed straight ahead of the subject wrt the chest.
		Assembly		*response;				// Similar to the line of targets. Used to show the subject what was his or her repsonse.
		Assembly		*projectiles;			// Another way of showing the subject his or her response. These can be shot out of the tool.
		Assembly		*multiProjectile;
		Assembly		*monoProjectile;

		Yoke			*hmd;					// Collection of objects that move with the head.
		Glasses			*glasses;				// A frame around the viewport into the virtual scene that moves with the head.
		Assembly		*headTiltPrompt;		// Shows the subject which direction to turn the head when the error is large.
		Assembly		*gazeLaser;				// A virtual laser pointer that moves with the line of gaze, making it easier to center the gaze at the start of a trial.
		FuzzyPointer	*fuzzyLaser;

		Assembly		*successIndicator;		// Shown briefly to indicate successful completion of a trial (currently not used).


		Assembly		*vTool;					// Full-fledged tool used in V-V and K-V. It is at a fixed locatino wrt the head and rolls with mouse movement.

		Yoke			*hand;					// An collection of objects that move with the hand.
		Assembly		*kTool;					// A tool that allows pointing the hand in pitch and yaw, but without indication about roll.
		Assembly		*vkTool;				// A tool that shows the hand's orientation. Used in V-VK and K-VK or in training for K-K.
		Assembly		*kkTool;				// A tool that is used to drive the hand to the target orientation in K-K.

		Assembly		*handRollPrompt;		// An arrow around the tool saying which way to turn.
		Sphere			*lowerHandPrompt;		// A cloud around the tool that is activated to remind the subject to lower the hand.
		Assembly		*wristZone;				// A transparent cloud used to prompt the subject to place the hand for K.

		OpenGLObject	*selectedTool;			// Used by the projectile handler. Sometimes the tool is at the hand, but when doing V-V it
												// is along the viewing axis. So we expect each task handler to set this to the appropriate one.

		Assembly		*room;					// A collection of objects that make up the fixed visual environment.
		Cylinder		*tunnel;				// Part of the room, but it allows us to access it directly to change its color.
		Assembly		*starrySky;				// Backgrounds that can be seen at the end of the tunnel.
		Assembly		*darkSky;


		// Objects used in third-person views of the virtual world.
		Assembly		*head;
		Assembly		*torso;
		// Representations of the CODA marker structures.
		MarkerStructureGLObject		*hmdStructure;
		MarkerStructureGLObject		*handStructure;
		MarkerStructureGLObject		*chestStructure;

	public: 

		GraspGLObjects( void ) {}

		void CreateTextures( void );
		void SetLighting( void );

		// Use color to guide the subject to a specified roll angle.
		void SetColorByRollError( OpenGLObject *object, double roll_error, double sweet_zone );

		// Need to be able to change the color of the fingers according to the position of the hand.
		// OpenGLObjects does not provide an easy way to override the color, so this hack lets us do it.
		void SetHandColor( Assembly *hand, bool enabled );
		// Need to be able to enable or disable a laser attached to a hand.
		void EnableHandLaser( Assembly *hand );
		void DisableHandLaser( Assembly *hand );
		void SetHandLaserEccentricity( Assembly *hand, double projection );
			

		// Create all the objects needed for VR.
		void CreateVRObjects( void );
		// Place objects at default locations.
		void PlaceVRObjects( void );
		// Draw the VR objects. Only those that are currently active will be drawn.
		void GraspGLObjects::DrawVR( void );

		Assembly *CreateVisualTool( void );
		Assembly *CreateKinestheticTool( void );
		Assembly *CreateLaserPointer(void);
		FuzzyPointer *CreateFuzzyLaserPointer( void );
		Assembly *CreateProjectiles( int fingers );
		Yoke	 *CreateHand(void);
		Yoke	 *CreateHUD(void);
		Assembly *CreateZone( void );

		Assembly *CreateOrientationTarget( void );
		Assembly *CreatePositionOnlyTarget( void );
		Assembly *CreateResponse( void );
		Assembly *CreateTiltPrompt( void );
		Assembly *CreateSuccessIndicator( void );
		Assembly *CreateIndicator( Texture *texture, double hole_radius = 35.0 );
		Assembly *CreateRoom( void );
		Assembly *CreateStarrySky( void );
		Assembly *CreateDarkSky( void );
		Glasses  *CreateGlasses(void);

		// Objects that are not used for VR in the HMD, but may be used
		// to create other 3D visual scenes.
		void CreateAuxiliaryObjects( void );
		Assembly *CreateHead( void );
		Assembly *CreateTorso( void );

		// Create an assembly that has spheres at the location of each marker,
		// as defined by a rigid body definition file.
		MarkerStructureGLObject *CreateRigidBodyMarkerStructure( char *rigid_body_model_file );
		MarkerStructureGLObject *GraspGLObjects::CreateHmdMarkerStructure ( char *model_file );
		MarkerStructureGLObject *GraspGLObjects::CreateHandMarkerStructure ( char *model_file );
		MarkerStructureGLObject *GraspGLObjects::CreateChestMarkerStructure ( char *model_file );

		void DrawHead( TrackerPose *pose = nullptr );
		void DrawTorso( TrackerPose *pose = nullptr );
		void DrawBody( TrackerPose *pose = nullptr );

	protected:
		~GraspGLObjects() {}

	};
};
