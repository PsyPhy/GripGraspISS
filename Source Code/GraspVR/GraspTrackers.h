///
/// Module:	GraspTrackers
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		30 May 2016
/// Modification History:	see https://github.com/PsyPhy/GRASPonISS
///
/// Copyright (c) 2016 PsyPhy Consulting
///

#pragma once

#include "../Useful/Useful.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"

#include "../OculusInterface/OculusInterface.h"

// Include 3D and 6D tracking capabilities.
#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaRTnetTracker.h"

#include "../Trackers/PoseTrackers.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../Trackers/CascadePoseTracker.h"
#include "../OculusInterface/OculusPoseTracker.h"
#include "../OculusInterface/OculusCodaPoseTracker.h"
#include "../OculusInterface/MousePoseTrackers.h"

namespace Grasp {

	using namespace PsyPhy;

	class GraspTrackers : public VectorsMixin {

	protected:
		static double	mouseGain;
		static Pose		handPoseVV;
		static double	arrowGain;
		static Pose		chestPoseSim;
		OculusMapper	*oculusMapper;

	public: 

		PoseTracker *hmdTracker;
		PoseTracker *handTracker;
		PoseTracker *chestTracker;
		PoseTracker *mouseTracker;

		GraspTrackers() {}
		virtual void Initialize( void ) = 0;
		virtual void Update( void );
		virtual void Release( void );
		virtual void WriteDataFiles( char *filename_root ){};

	protected:
		~GraspTrackers() {}

	};

	class GraspSimTrackers : public GraspTrackers {
	public:

		// We will need a mouse tracker of some kind to do the V-V protocol.
		MouseRollPoseTracker *mouseRollTracker;

		virtual void Initialize( void );
		GraspSimTrackers( OculusMapper *mapper ) {
			oculusMapper = mapper;
		}
		~GraspSimTrackers( void ) {}
	};


	class GraspDexTrackers : public GraspTrackers {

	protected:

		static const int nMarkers = 24;
		static const int nCodaUnits = 2;

		// A device that records 3D marker positions.
		// Those marker positions will also drive the 6dof pose trackers.
		CodaRTnetTracker codaTracker;

		// Buffers to hold the most recent frame of marker data.
		MarkerFrame markerFrame[MAX_CASCADED_TRACKERS];

		// CodaPoseTrackers compute the pose from a frame of Coda data and a rigid body model.
		// We use one for each Coda unit, but we could use more.
		CodaPoseTracker *hmdCodaPoseTracker[MAX_CASCADED_TRACKERS];
		CodaPoseTracker *handCodaPoseTracker[MAX_CASCADED_TRACKERS];
		CodaPoseTracker *chestCodaPoseTracker[MAX_CASCADED_TRACKERS];

		// CascadePoseTrackers group together multiple PoseTrackers for the same entity.
		// It sequentially tries to get the pose from one after another, stopping as soon as it gets one.
		// We use this mechanism to combine the pose info from each coda unit, because it it better to
		// compute the pose from markers positions measured by the same coda unit. Priority is therefore
		// given to the first coda in the list. Others are used only as needed.
		CascadePoseTracker *hmdCascadeTracker;
		CascadePoseTracker *handCascadeTracker;
		CascadePoseTracker *chestCascadeTracker;

		// For the HMD we can combine pose information from both the HMD and a Coda tracker.
		OculusCodaPoseTracker *oculusCodaPoseTracker;

		// We will need a mouse tracker of some kind to do the V-V protocol.
		MouseRollPoseTracker *mouseRollTracker;

	public:

		void Initialize( void );
		void Update( void );
		void Release( void );
		void WriteDataFiles( char *filename_root );

		GraspDexTrackers( OculusMapper *mapper ) {
			oculusMapper = mapper;
		}
		~GraspDexTrackers( void ) {}
	};

}


