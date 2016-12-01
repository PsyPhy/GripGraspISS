// GraspTelemetryDaemon.h : main project file.

#pragma once

#include "../DexServices/TMdata.h"
#include "../DexServices/DexServices.h"
#include "../../../GripMMI/GripSourceCode/Grip/GripPackets.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

typedef struct {
	double absoluteTime;
	f32 fillTime;
	u32 globalCount;
	u32 objectStateBits;
	PsyPhy::TrackerPose	HMD;
	PsyPhy::TrackerPose	codaHMD;
	PsyPhy::TrackerPose hand;
	PsyPhy::TrackerPose chest;
	PsyPhy::TrackerPose	mouse;
} GraspRealtimeDataSlice;

#define MAX_SLICES	(24*60*60*10)
extern GraspRealtimeDataSlice graspDataSlice[MAX_SLICES];
extern unsigned int nDataSlices;

void ExtractGraspRealtimeDataInfo( GraspRealtimeDataSlice grasp_data_slice[], EPMTelemetryPacket &packet );
int GetGraspRT( GraspRealtimeDataSlice grasp_data_slice[], int max_slices, char *filename_root );