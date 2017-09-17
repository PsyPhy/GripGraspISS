// GraspTrackerDaemon.h

// Coda tracker and equivalents.

#include "../Useful/fOutputDebugString.h"
#include "../Useful/fMessageBox.h"
#include "../Trackers/CodaRTnetContinuousTracker.h"
#include "../Trackers/PoseTrackers.h"
// GraspTrackerDaemon will output data on this port.
#define TRACKER_DAEMON_PORT		1706
#define TRACKER_DAEMON_PORTS	4
#define TRACKER_DAEMON_COMMAND_PORT	"2106"

// Currently I broadcast only to the local host because everything is running
// on the same machine. Note that when I had 'localhost" instead of "127.0.0.1", things 
// ran slower when my wifi was connected. I don't know what would happen when the ethernet
// is connected (as it will be for Grasp) but I am hoping that using 127.0.0.1 will restrict
// the broadcast to the local machine.
#define TRACKER_BROADCAST_ADDRESS "127.0.0.1"

#ifdef  __cplusplus
extern "C" {
#endif

// The data output by GraspTrackerDaemon.
typedef struct {
	unsigned long count;
	PsyPhy::TrackerPose	codaHmd;
	PsyPhy::TrackerPose hand;
	PsyPhy::TrackerPose chest;
	int	nUnits;
	MarkerFrame frame[MAX_UNITS];
	PsyPhy::Vector3 alignmentOffset[MAX_UNITS];
	PsyPhy::Matrix3x3 alignmentRotation[MAX_UNITS];
} GraspTrackerRecord;

#ifdef  __cplusplus
}
#endif