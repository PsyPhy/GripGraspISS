#pragma once

// The following defines are used to indicate progress through different
// phases of the alignment process. These are sent as the sub-step indicator to 
// ground via DEX using DexServices.
#define STARTUP_VISIBILITY	10
#define VISIBILITY			15
#define SHUTDOWN_VISIBILITY 20
#define ACQUIRE_PREALIGN	25
#define COMPUTE_ALIGNMENT	30
#define INSTALL_ALIGNMENT	40
#define ACQUIRE_ALIGNED		45
#define SHOW_RESULTS		50
#define ALIGNMENT_ERROR		9999
#define ABORT_REQUESTED		8888
#define RESET_REQUESTED		7777
	
