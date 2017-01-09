// GraspMMIMixin.h

#pragma once

#include "../DexServices/TMdata.h"
#include "../DexServices/DexServices.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "../../../GripMMI/GripSourceCode/Grip/GripPackets.h"

#define CHEST_STRUCTURE	0
#define HAND_STRUCTURE	1
#define HMD_STRUCTURE	2

namespace GraspMMI {

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
		double hmdRotationAngle;		// Rotation of the HMD away from straight ahead, including pitch and yaw, in degrees.
		double hmdRollAngle;	// Rotation of the HMD around the roll axis.
	} GraspRealtimeDataSlice;

	typedef struct {
		double		absoluteTime; 
		double		stepID;
		double		taskID;
		double		protocolID;
		double		userID;
		int		visibleMarkers[3];
	} GraspHousekeepingSlice;

	void ExtractGraspRealtimeDataInfo( GraspRealtimeDataSlice grasp_data_slice[], EPMTelemetryPacket &packet );
	int GetGraspRT( GraspRealtimeDataSlice grasp_data_slice[], int max_slices, char *filename_root );
	int GetHousekeepingTrace( GraspHousekeepingSlice *trace, int max_slices, char *filename_root );

}
