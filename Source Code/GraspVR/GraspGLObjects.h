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
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"

#include "../Trackers/PoseTrackers.h"

namespace Grasp {

		using namespace PsyPhy;

		class GraspGLObjects : public VectorsMixin {

		private:
		
			// Textures that are used to decorate certain objects.
			// These files have to be in the execution directory.
			static const char *GraspGLObjects::wall_texture_bitmap;
			static const char *GraspGLObjects::sky_texture_bitmap;
			static const char *GraspGLObjects::references_texture_bitmap;
			
			// Dimensions of the room.
			static const double GraspGLObjects::room_radius;
			static const double GraspGLObjects::room_length;
			static const double GraspGLObjects::wall_thickness;
			static const double GraspGLObjects::reference_bars;
			static const double GraspGLObjects::reference_bar_radius;

			// Dimensions of objects in the room.
			static const Vector3 head_shape;
			static const Vector3 torso_shape;

			static const Vector3 target_location;
			static const Vector3 prompt_location;
			static const double prompt_radius;
			static const double target_ball_radius;
			static const double target_ball_spacing;
			static const int target_balls;
			static const double finger_length;

		protected:

			Texture			*wall_texture;		// The texture that is applied to the walls.
			Texture			*sky_texture;
			Texture			*references_texture;

		public:

			Assembly		*target;			// Shows the target orientation.
			Assembly		*response;			// Shows the final response.
			Assembly		*laserPointer;
			Assembly		*tiltPrompt;

			Assembly		*tool;				// An object that moves in the world.
			Assembly		*sky;				// A background to be seen at the end of the tunnel.
			Assembly		*room;

			Assembly		*head;
			Assembly		*torso;

		public: 

			GraspGLObjects( void ) {}

			void CreateTextures( void );
			void CreateObjects( void );
			void SetLighting( void );

			Assembly *CreateTool( void );
			Assembly *CreateTarget( void );
			Assembly *CreateTiltPrompt( void );
			Assembly *CreateRoom( void );
			Assembly *CreateSky( void );
			Assembly *CreateHead( void );
			Assembly *CreateTorso( void );

			void DrawTarget( TrackerPose *pose = nullptr );
			void DrawTiltPrompt( TrackerPose *pose = nullptr );
			void DrawTool( TrackerPose *pose = nullptr );
			void DrawRoom( TrackerPose *pose = nullptr );
			void DrawSky( void );

			void DrawHead( TrackerPose *pose = nullptr );
			void DrawTorso( TrackerPose *pose = nullptr );
			void DrawBody( TrackerPose *pose = nullptr );

		protected:
			~GraspGLObjects() {}

		};
};
