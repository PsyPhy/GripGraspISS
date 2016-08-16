// GraspTrackerDaemon.h

// Coda tracker and equivalents.
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/PoseTrackers.h"

// GraspTrackerDaemon will output data on this port.
#define TRACKER_DAEMON_PORT		1706
// I thought that I would broadcast to anyone who wants to listen.
// But I haven't mastered that technique and for the current application 
// everything runs on one machine. So I just put it as localhost.
#define TRACKER_BROADCAST_ADDRESS	"localhost"

// The data output by GraspTrackerDaemon.
typedef struct {
	unsigned long count;
	PsyPhy::TrackerPose	hmd;
	PsyPhy::TrackerPose hand;
	PsyPhy::TrackerPose chest;
	int	nUnits;
	MarkerFrame frame[MAX_UNITS];
} GraspTrackerRecord;
