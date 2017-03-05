// GraspMMIMixin.h

#pragma once

#include "../DexServices/TMdata.h"
#include "../DexServices/DexServices.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../Grip/GripPackets.h"

#define CHEST_STRUCTURE	0
#define HAND_STRUCTURE	1
#define HMD_STRUCTURE	2
#define MARKER_STRUCTURES	3

namespace GraspMMI {

	// Data contained in realtime data packets.
	typedef struct {
		// Transmitted Items
		double absoluteTime;
		f32 fillTime;
		u32 globalCount;
		u32 objectStateBits;
		PsyPhy::TrackerPose	HMD;
		PsyPhy::TrackerPose	codaHMD;
		PsyPhy::TrackerPose hand;
		PsyPhy::TrackerPose chest;
		PsyPhy::TrackerPose	mouse;
		// Computed Items
		double hmdRotationAngle;	// Rotation of the HMD away from straight ahead, including pitch and yaw, in degrees.
		double hmdRollAngle;		// Rotation of the HMD around the roll axis.
	} GraspRealtimeDataSlice;

	// Data contained in housekeeping packets.
	// Most of the values are actually integers, but we store them as doubles because
	// we want to plot them against time and time is a double. All my graph
	// routines plot float against float, int against int, etc. so we can't mix types
	// between the X and Y axes.
	typedef struct {
		double		absoluteTime; 
		double		stepID;
		double		taskID;
		double		protocolID;
		double		userID;
		double		visibleMarkers[MARKER_STRUCTURES];
		unsigned int	scriptEngine;
	} GraspHousekeepingSlice;

	void ExtractGraspRealtimeDataInfo( GraspRealtimeDataSlice grasp_data_slice[], EPMTelemetryPacket &packet );
	int GetGraspRT( GraspRealtimeDataSlice grasp_data_slice[], int max_slices, char *filename_root );
	int GetHousekeepingTrace( GraspHousekeepingSlice *trace, int max_slices, char *filename_root );

}
