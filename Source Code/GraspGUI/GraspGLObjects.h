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

#include "../Trackers/PoseTrackers.h"

namespace Grasp {

		using namespace PsyPhy;

		class GraspGLObjects : public VectorsMixin {

		private:
		
			// Textures that are used to decorate certain objects.
			// These files have to be in the execution directory.
			static const char *GraspGLObjects::wall_texture_bitmap;
			static const char *GraspGLObjects::sky_texture_bitmap;
			
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
			static const double target_ball_radius;
			static const int target_balls;
			static const double finger_length;

		protected:

			Texture			*wall_texture;		// The texture that is applied to the walls.
			Texture			*sky_texture;

		public:

			Assembly		*target;			// Shows the target orientation.
			Assembly		*response;			// Shows the final response.
			Assembly		*laserPointer;
			Assembly		*tool;				// An object that moves in the world.
			Assembly		*sky;				// A background to be seen at the end of the tunnel.
			Assembly		*room;

			Assembly		*head;
			Assembly		*torso;

		public: 
			GraspGLObjects( void ) {}

			Assembly *CreateTool( void );
			Assembly *CreateTarget( void );
			Assembly *CreateRoom( void );
			Assembly *CreateSky( void );
			Assembly *CreateHead( void );
			Assembly *CreateTorso( void );

			void CreateTextures( void );
			void CreateObjects( void );

			void DrawTarget( void );
			void DrawTool( TrackerPose *pose = &NullTrackerPose );
			void DrawRoom( void );
			void DrawSky( void );

			void DrawHead( TrackerPose *pose = &NullTrackerPose );
			void DrawTorso( TrackerPose *pose = &NullTrackerPose );
			void DrawBody( TrackerPose *pose = &NullTrackerPose );

		protected:
			~GraspGLObjects() {}

		};
};
