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

static char *StructureLabel[MARKER_STRUCTURES] =  { "CHEST", "HAND", "HMD" };

namespace GraspMMI {

	// Data contained in realtime data packets.
	typedef struct {

		// Transmitted Items
		double absoluteTime;
		f32 fillTime;
		u32 globalCount;
		u32 objectStateBits;
		PsyPhy::TrackerPose	HMD;
		PsyPhy::TrackerPose hand;
		PsyPhy::TrackerPose chest;
		
		unsigned char	codaUnit;
		MarkerFrame		codaFrame[MAX_UNITS];
		f32				clientTime;
		unsigned char	clientData[GRASP_RT_CLIENT_BYTES];

		enum { NONE, GRASP, GRASPGUI, ALIGNPRE, ALIGNPOST } clientType;

		// These are the key poses that come from the GRASP client data.
		unsigned long		enableBits;
		unsigned long		spinnerBits;
		double				targetOrientation;
		PsyPhy::TrackerPose	headPose;
		PsyPhy::TrackerPose	handPose;
		PsyPhy::TrackerPose	chestPose;
		Quaternion			rollQuaternion;

		// These are the information about the CODA alignment transformations.
		Vector3			alignmentOffset[MAX_UNITS];
		Matrix3x3		alignmentRotation[MAX_UNITS];

		// Computed Items
		double hmdRotationAngle;	// Rotation of the HMD away from straight ahead, including pitch and yaw, in degrees.
		double hmdRollAngle;		// Rotation of the HMD around the roll axis.
		double handRotationAngle;	// Rotation of the Hand away from straight ahead, including pitch and yaw, in degrees.
		double handRollAngle;		// Rotation of the Hand around the roll axis.
		double chestRotationAngle;	// Rotation of the Chest away from straight ahead, including pitch and yaw, in degrees.
		double chestRollAngle;		// Rotation of the Chest around the roll axis.
		double torsoRollAngle;		// Rotation of the line from Chest to HMD around the roll axis.

	} GraspRealtimeDataSlice;

	typedef struct {
		unsigned int	acquisitionID;
		unsigned int	packetCount;
		GraspRealtimeDataSlice dataSlice[GRASP_RT_SLICES_PER_PACKET];
	} GraspRealtimeDataInfo;

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
		double		visibleMarkers[MAX_UNITS][MARKER_STRUCTURES];
		unsigned int	scriptEngine;
	} GraspHousekeepingSlice;

	int GetGraspRT( GraspRealtimeDataSlice grasp_data_slice[], int max_slices, char *filename_root );
	int GetHousekeepingTrace( GraspHousekeepingSlice *trace, int max_slices, char *filename_root );
	void ExtractGraspRealtimeDataInfo( GraspRealtimeDataInfo &info, EPMTelemetryPacket &epm_packet );

}
