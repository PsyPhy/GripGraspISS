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

		protected:
		
			// Textures that are used to decorate certain objects.
			// These files have to be in the execution directory.
			static const char *wall_texture_bitmap;
			static const char *sky_texture_bitmap;
			static const char *references_texture_bitmap;
			
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
			static const double target_ball_radius;
			static const double finger_ball_radius;
			static const double target_ball_spacing;
			static const int target_balls;
			static const double finger_length;

			static const double headRollTolerance;
			static const double kkTolerance;

		protected:

			Texture			*wall_texture;		// The texture that is applied to the walls.
			Texture			*sky_texture;
			Texture			*references_texture;

		public:

			Assembly		*orientationTarget;			// Shows the target orientation.
			Assembly		*positionOnlyTarget;		// Shows the center of the target location.
			Assembly		*response;

			Assembly		*tiltPrompt;

			Assembly		*successIndicator;			// Shown briefly to indicate successful completion of a trial.
			Assembly		*timeoutIndicator;			// Shown when the subject takes too long to do something.

			Yoke			*hand;				// An collection of objects that move with the hand.
			Assembly		*vTool;				// Full-fledged tool. One can see its orientation.
			Assembly		*kTool;				// A tool that allows pointing the hand in pitch and yaw, but without indication about roll.
			Assembly		*kkTool;			// A tool that is used to drive the hand to the target orientation in K-K.

			Assembly		*projectiles;		// These can be shot out of the tool.

			Assembly		*room;
			Cylinder		*tunnel;			// Part of the room, but it allows us to access it directly to change its color.
			Assembly		*starrySky;			// Backgrounds that can be seen at the end of the tunnel.
			Assembly		*darkSky;
			Glasses			*glasses;			// A frame around the viewport into the virtual scene that moves with the head.

			Assembly		*head;
			Assembly		*torso;

			MarkerStructureGLObject		*hmdStructure;
			MarkerStructureGLObject		*handStructure;
			MarkerStructureGLObject		*chestStructure;



		public: 

			GraspGLObjects( void ) {}

			void CreateTextures( void );
			void SetLighting( void );

			// Create all the objects needed for VR.
			void CreateVRObjects( void );
			// Place objects at default locations.
			void PlaceVRObjects( void );
			// Draw the VR objects. Only those that are currently active will be drawn.
			void GraspGLObjects::DrawVR( void );

			Assembly *CreateVisualTool( void );
			Assembly *CreateKinestheticTool( void );
			Assembly *CreateLaserPointer(void);
			Assembly *CreateProjectiles(void);
			Yoke	 *CreateHand(void);

			Assembly *CreateOrientationTarget( void );
			Assembly *CreatePositionOnlyTarget( void );
			Assembly *CreateResponse( void );
			Assembly *CreateTiltPrompt( void );
			Assembly *CreateSuccessIndicator( void );
			Assembly *CreateTimeoutIndicator( void );
			Assembly *CreateHeadMisalignIndicator( void );
			Assembly *CreateRoom( void );
			Assembly *CreateStarrySky( void );
			Assembly *CreateDarkSky( void );
			Glasses  *CreateGlasses(void);

			bool SetColorByRollError( OpenGLObject *object, double desired_angle, double epsilon );
			bool ColorKK( double desired_angle );
			bool ColorGlasses( double desired_angle );

			void DrawOrientationTarget( TrackerPose *pose = nullptr );
			void DrawPositionOnlyTarget(  TrackerPose *pose = nullptr  );
			void DrawResponse(  TrackerPose *pose = nullptr  );
			void DrawTiltPrompt( TrackerPose *pose = nullptr );
			void DrawHand( TrackerPose *pose = nullptr );
			void DrawProjectiles(TrackerPose *pose = nullptr);
			void DrawRoom( TrackerPose *pose = nullptr );
			void DrawStarrySky( void );
			void DrawDarkSky( void);
			void DrawGlasses( TrackerPose *pose = nullptr );

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
