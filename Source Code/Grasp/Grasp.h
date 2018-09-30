#pragma once

#define MAX_GRASP_TRIALS 64
#define MANUAL_BLOCK_INTERRUPTION -2
#define MAX_RETRIES_EXCEEDED -3

// The code in GraspTaskManager::FillClientData() implicitly defines the bit mask for each object.
// Here I define compiler constants that go along with those definitions.
// It would have been cleaner to define the constants (or an enum)
// and then use them in GraspTaskManager::FillClientData(), but I don't want to change
// the code that actually runs Grasp for the moment.

#define ORIENTATION_TARGET ( 1 )
#define POSITION_ONLY_TARGET ( 1 << 1 )
#define STRAIGHT_AHEAD_TARGET ( 1 << 2 )
#define RESPONSE ( 1 << 3 )
#define PROJECTILES ( 1 << 4 )
#define MULTI_PROJECTILE ( 1 << 5 )
#define MONO_PROJECTILE ( 1 << 6 )
#define GLASSES ( 1 << 7 )
#define TILT_PROMPT ( 1 << 8 )
#define GAZE_LASER ( 1 << 9 )
#define HAND_LASER ( 1 << 10 )
#define SUCCESS_INDICATOR ( 1 << 11 )
#define V_TOOL ( 1 << 12 )
#define K_TOOL ( 1 << 13 )
#define VK_TOOL ( 1 << 14 )
#define KK_TOOL ( 1 << 15 )
#define HAND_ROLL_PROMPT ( 1 << 16 )
#define LOWER_HAND_PROMPT ( 1 << 17 )
#define WRIST_ZONE_PROMPT ( 1 << 18 )
#define GRASP_ROOM ( 1 << 19 )
#define GRASP_TUNNEL ( 1 << 20 )
#define STARRY_SKY ( 1 << 21 )
#define DARK_SKY ( 1 << 22 )

#define READY_TO_START_INDICATOR ( 1 << 0 )
#define BLOCK_COMPLETED_INDICATOR ( 1 << 1 )
#define LOWER_HAND_INDICATOR ( 1 << 2 )
#define RAISE_HAND_INDICATOR ( 1 << 3 )
#define RAISE_ARM_TIMEOUT_INDICATOR ( 1 << 4)
#define HEAD_MISALIGNED_INDICATOR ( 1 << 5 )
#define MANUAL_REJECT_INDICATOR ( 1 << 6 )
#define INVALIDATE_TRIAL_INDICATOR ( 1 << 7 )
#define HEAD_ALIGN_TIMEOUT_INDICATOR ( 1 << 8 )
#define TIMEOUT_INDICATOR ( 1 << 9 )
#define RESPONSE_TIMEOUT_INDICATOR ( 1 << 10 )
#define HAND_ROTATE_TIMEOUT_INDICATOR ( 1 << 11 )
#define HAND_TOO_SOON_INDICATOR ( 1 << 12 )
#define HAND_SHOULD_NOT_INDICATOR ( 1 << 13 )
#define STRAIGHTEN_HEAD_INDICATOR ( 1 << 14 )
#define VR_COMPLETED_INDICATOR (1 << 15 )
#define DEMO_STATE_INDICATOR ( 1 << 16 )
#define INTERRUPT_INDICATOR ( 0x7ffc )

namespace Grasp {

	typedef enum { unknown = 0, VTOV, VTOK, KTOK, VTOVK, VKTOVK, QUITVR, DEMO } Paradigm;
	static char *GraspParadigmString[] = { "????", "VtoV", "VtoK", "KtoK", "VtoVK", "VKtoVK", "QUITVR", "DEMO" };

	typedef enum { NullState = 0, StartBlock = 10, BlockCompleted = 19, 
					StartTrial = 20, 
					StraightenHead = 31, AlignHead = 32, 
					PresentTarget = 33, TiltHead = 34, ObtainResponse = 35, 
					ProvideFeedback = 36, TrialCompleted = 39, 
					TrialInterrupted = 40, VRCompleted = 50, Demo = 98, ExitStateMachine = 99 } GraspTrialState;

	typedef enum { RAISE_HAND_TIMEOUT = 1, LOWER_HAND_TIMEOUT, RAISED_HAND_VIOLATION, HAND_TOO_SOON, ALIGN_HAND_TIMEOUT, HEAD_ALIGNMENT_TIMEOUT, HEAD_TILT_TIMEOUT, HEAD_MISALIGNMENT, RESPONSE_TIMEOUT, MANUAL_REJECT_TRIAL, INVALIDATE_TRIAL } Anomaly;
	typedef enum { MANUAL_STRAIGHTEN, CODA_STRAIGHTEN, CHEST_STRAIGHTEN } StraightenHeadMethod;

	typedef struct {

		char ID[8];

		unsigned long enableBits;
		unsigned long spinnerBits;

		CompactPose	headPose;
		CompactPose	handPose;
		CompactPose	chestPose;
		float rollQuaternion[4];

		short	targetHeadTiltD;
		short	targetOrientationD;
		short	responseHeadTiltD;
		short	currentTrial;
		float	conflictGain;

		Paradigm		paradigm;
		GraspTrialState	currentState;
		bool	provideFeedback;

	} GraspClientData;

}
